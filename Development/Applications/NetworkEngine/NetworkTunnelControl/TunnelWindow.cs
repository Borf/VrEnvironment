using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NetworkTunnelControl
{
	public partial class TunnelWindow : Form
	{
		private SessionSelect sessionSelect;

		public TunnelWindow()
		{
			InitializeComponent();
		}

		public TunnelWindow(SessionSelect sessionSelect)
		{
			InitializeComponent();
			this.sessionSelect = sessionSelect;
		}

		private void TunnelWindow_FormClosed(object sender, FormClosedEventArgs e)
		{
			if(sessionSelect != null)
				Application.Exit();
		}


		public class MyTreeNode : TreeNode
		{
			public MyTreeNode(string name) : base(name) { }
			public dynamic data;
		}

		MyTreeNode buildTree(dynamic node)
		{
			MyTreeNode n = new MyTreeNode((string)node.name);
			n.data = node;
			n.Expand();

			if(node.children != null)
				foreach(var c in node.children)
					n.Nodes.Add(buildTree(c));
			return n;
		}

		private void TunnelWindow_Load(object sender, EventArgs e)
		{
			Connection.callbacks["scene/get"] = (json) =>
			{
				Invoke((Action) delegate
				{
					treeView1.Nodes.Clear();
					TreeNode rootNode = buildTree(json);
					treeView1.Nodes.Add(rootNode);
				});
			};
			Connection.callbacks["tunnel/close"] = (json) =>
			{
				Invoke((Action)delegate
			   {
				   sessionSelect.Show();
				   sessionSelect = null;
				   Close();
			   });
			};

			Connection.sendTunnel("scene/get", null);

			//button7_Click(sender, e);
		}

		private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
		{
			if (treeView1.SelectedNode == null)
				return;
			var c = ((MyTreeNode)treeView1.SelectedNode).data;

			Node n = new Node();
			n.name = c.name;
			n.guid = c.uuid;

			if (c.components != null && c.components.Count > 0)
			{
				for (int i = 0; i < c.components.Count; i++)
				{
					n.components.Add(new Component() { type = Component.Type.Transform });
				}

			}
			propertyGrid1.SelectedObject = n;

		}

		private void button1_Click(object sender, EventArgs e)
		{
			string uuid = "";
			AutoResetEvent blocker = new AutoResetEvent(false);
			Connection.callbacks["scene/node/add"] = (data) =>
			{
				uuid = data["uuid"];
				blocker.Set();
			};


			Connection.sendTunnel("scene/node/add",
			new
			{
				name = "Surface",
				components = new
				{
					transform = new
					{
						position = new[] { 0, 0, 0 },
						scale = 100
					},
					model = new
					{
						file = "data/TienTest/biker/models/ground/landscape.obj"
					}
				}
			});
			blocker.WaitOne()
				
				;
			Connection.sendTunnel("scene/node/add",
			new
			{
				name = "forest",
				components = new
				{
					transform = new
					{
						position = new[] { 0, 0, 0 },
						scale = 1
					}
				}
			});
			blocker.WaitOne();

			Connection.callbacks.Remove("scene/node/add");
			string flooruuid = uuid;

			Random r = new Random();
			for (int i = 0; i < 100; i++)
			{
				Connection.sendTunnel("scene/node/add",
				new
				{
					name = "Tree",
					parent = flooruuid,
					components = new
					{
						transform = new
						{
							position = new[] { r.NextDouble() * 20-10, 0, -2 - r.NextDouble() * 20 },
							rotation = new[] { 0, r.NextDouble() * 360, 0 },
							scale = 1 + 0.4 * r.NextDouble()
						},
						model = new
						{
							file = "data/TienTest/biker/models/trees/2/tree1.obj",
							cullbackfaces = false
						}
					}
				});
			}


			Connection.sendTunnel("scene/get", null);
		}

		private void btnRemove_Click(object sender, EventArgs e)
		{
			if (treeView1.SelectedNode == null)
				return;
			var c = ((MyTreeNode)treeView1.SelectedNode).data;

			string uuid = c.uuid;
			Connection.sendTunnel("scene/node/delete", new { id = uuid });
			Connection.sendTunnel("scene/get", null);
		}

		private void timeOfDay_Scroll(object sender, EventArgs e)
		{
			int value = timeOfDay.Value;
			float fac = value / (float)timeOfDay.Maximum;
			fac = fac * 24;

			Connection.sendTunnel("scene/skybox/settime", new { time = fac });

		}

		private void button1_Click_1(object sender, EventArgs e)
		{
			Random r = new Random();
			float[,] heights = new float[32,32];
			for (int x = 0; x < 32; x++)
				for (int y = 0; y < 32; y++)
					heights[x, y] = 2 + (float)(Math.Cos(x/5.0) + Math.Cos(y/5.0));


			Connection.sendTunnel("scene/terrain/add",
			new
			{
				size = new[] { 32, 32 },
				heights = heights.Cast<float>().ToArray()
			});

			Connection.sendTunnel("scene/node/add",
			new
			{
				name = "floor",
				components = new
				{
					transform = new
					{
						position = new[] { -16, 0, -16 },
						scale = 1
					},
					terrain = new
					{

					}
				}
			});




			Connection.sendTunnel("scene/get", null);
		}

		private void btnStuff_Click(object sender, EventArgs e)
		{
			using (Bitmap heightmap = new Bitmap("../../heightmap.png"))
			{
				Connection.sendTunnel("scene/reset", null);
				Connection.sendTunnel("pause", null);

				float[,] heights = new float[heightmap.Width, heightmap.Height];
				for (int x = 0; x < heightmap.Width; x++)
					for (int y = 0; y < heightmap.Height; y++)
						heights[x, y] = (heightmap.GetPixel(x, y).R / 256.0f) * 25.0f;

				Connection.sendTunnel("scene/terrain/add",
				new
				{
					size = new[] { heightmap.Width, heightmap.Height },
					heights = heights.Cast<float>().ToArray()
				});
				

				string terrainId = Connection.sendTunnelWait("scene/node/add",
				new
				{
					name = "floor",
					components = new
					{
						transform = new
						{
							position = new[] { -heightmap.Width/2, 0, -heightmap.Height/2 },
							scale = 1
						},
						terrain = new
						{

						}
					}
				}, data => data.uuid);
				Console.WriteLine("Terrain ID: " + terrainId);
				Connection.sendTunnel("scene/node/addlayer",
					new
					{
						id = terrainId,
						diffuse = "data/TienTest/textures/grass_diffuse.png",
						normal = "data/TienTest/textures/grass_normal.png",
						minHeight = -10.0f,
						maxHeight = 10.0f,
						fadeDist = 0.5f
					});

				Connection.sendTunnel("scene/node/addlayer",
					new
					{
						id = terrainId,
						diffuse = "data/TienTest/textures/ground_diffuse.png",
						normal = "data/TienTest/textures/ground_normal.png",
						minHeight = 10f,
						maxHeight = 50.0f,
						fadeDist = 0.5f
					});
				string bikeId = Connection.sendTunnelWait("scene/node/add",
				new
				{
					name = "bike",
					components = new
					{
						transform = new
						{
							position = new[] { 0, 0.15, 0 },
							scale = 0.01
						},
						model = new
						{
							file = "data/Networkengine/models/bike/bike_anim.fbx",
							animated = true,
							animation = "Armature|Fietsen"
						}
					}
				}, data => data.uuid);
				Console.WriteLine("Bike ID: " + bikeId);

				string cameraId = Connection.sendTunnelWait("scene/node/find",
					new { name = "Camera" }, data => data[0].uuid);
				Console.WriteLine("Camera ID: " + cameraId);


				Connection.sendTunnel("scene/node/update",
					new
					{
						id = cameraId,
						parent = bikeId,
						transform = new
						{
							scale = 100
						}
					});


				string treesId = Connection.sendTunnelWait("scene/node/add",
				new
				{
					name = "trees"
				}, data => data.uuid);
				Console.WriteLine("Trees ID: " + treesId);


				Random r = new Random();
				List<float[]> positions = new List<float[]>();
				for (int i = 0; i < 100; i++)
					positions.Add(new[] { (float)(r.NextDouble() * 80 - 40), 0.0f, (float)(r.NextDouble() * 80 - 40) });


				Connection.sendTunnelWait("scene/terrain/getheight", new
				{
					positions = positions.Select(a => new float[] { a[0], a[2] }).ToArray()
				}, data =>
				{
					for (int i = 0; i < 100; i++)
						positions[i][1] = data["heights"][i];
					return null;
				});


				for (int i = 0; i < 50; i++)
				{
					Connection.sendTunnel("scene/node/add",
					new
					{
						name = "Tree",
						parent = treesId,
						components = new
						{
							transform = new
							{
								position = positions[i],
								rotation = new[] { 0, r.NextDouble() * 360, 0 },
								scale = 1 + 0.4 * r.NextDouble()
							},
							model = new
							{
								file = "data/TienTest/biker/models/trees/2/tree"+r.Next(1, 7)+".obj",
								cullbackfaces = false
							}
						}
					});
				}
				

				float roundness = 10;
				float size = 20;
				string routeId = Connection.sendTunnelWait("route/add", new
				{
					nodes = new[]
					{
						new {   pos = new [] { size, 0, -size },    dir = new [] { roundness, 0, roundness } },
						new {   pos = new [] { size, 0, size },		dir = new [] {-roundness, 0, roundness } },
						new {   pos = new [] { -size, 0, size },    dir = new [] {-roundness, 0,-roundness } },
						new {   pos = new [] { -size, 0, -size },   dir = new [] { roundness, 0,-roundness } }
					}

				}, data => data.uuid);


				Connection.sendTunnel("route/follow", new
				{
					route = routeId,
					node = bikeId,
					speed = 2.0,
					rotate = "XZ",
					followHeight = true
				});


				Connection.sendTunnel("scene/road/add", new
				{
					route = routeId
				});


				string panelId = Connection.sendTunnelWait("scene/node/add",
				new
				{
					name = "panel",
					parent = bikeId,
					components = new
					{
						transform = new
						{
							position = new[] { -40, 115, 0 },
							rotation = new[] { -30, 90, 0 },
							scale = 100
						},
						panel = new
						{
							size = new[] { 0.2, 0.3 },
							resolution = new[] { 512, 512 }
						}
					}
				}, data => data.uuid);

				Connection.sendTunnel("scene/panel/clear", new
				{
					id = panelId,
				});


				List<double[]> lines = new List<double[]>();
				double step = Math.PI / 36;
				for (double f = 0; f < 2 * Math.PI; f += step)
					lines.Add(new[] { 256 + 200 * Math.Cos(f), 300 + 150 * Math.Sin(f), 256 + 200 * Math.Cos(f + step), 300 + 150 * Math.Sin(f + step), 0, 0, 0, 1 });

				lines.Add(new[] { 256, 300, 256 + 180 * Math.Cos(45 / 100.0 * 2 * Math.PI), 300 + 150 * Math.Sin(45 / 100.0 * 2 * Math.PI), 1, 0, 0, 1 });


				Connection.sendTunnel("scene/panel/drawlines", new
				{
					id = panelId,
					width = 1,
					lines = lines.ToArray()
				});

				Connection.sendTunnel("scene/panel/drawtext", new
				{
					id = panelId,
					text = "Speed: 10km/h",
					position = new[] { 50, 40 }
				});
				Connection.sendTunnel("scene/panel/drawtext", new
				{
					id = panelId,
					text = "Heartrate: 100bpm",
					position = new[] { 50, 80 }
				});
				Connection.sendTunnel("scene/panel/drawtext", new
				{
					id = panelId,
					text = "Power: 33%",
					position = new[] { 50, 120 }
				});

				Connection.sendTunnel("scene/panel/swap", new
				{
					id = panelId,
				});


				Connection.sendTunnel("play", null);
				Connection.sendTunnel("scene/get", null);
			}

		}

		private void button2_Click(object sender, EventArgs e)
		{
			Connection.sendTunnel("scene/reset", null);

			string panelId = Connection.sendTunnelWait("scene/node/add",
			new
			{
				name = "panel",
				components = new
				{
					transform = new
					{
						position = new[] { 0, 1, 0 },
						rotation = new[] { 0, 0, 0 },
						scale = 1
					},
					panel = new
					{
						size = new[] { 1, 1 },
						resolution = new[] { 512, 512 },
					}
				}
			}, data => data.uuid);

			Connection.sendTunnel("scene/node/moveto", new
			{
				id = panelId,
				position = new[] { 0, 1, -50 },
				speed = 0.1
			});


			int count = 0;

			var t = new System.Windows.Forms.Timer();
			t.Tick += new EventHandler((a, b) =>
			{
				Connection.sendTunnel("scene/panel/clear", new
				{
					id = panelId,
				});


				List<double[]> lines = new List<double[]>();
				double step = Math.PI / 36;
				for(double f = 0; f < 2 * Math.PI; f+=step)
					lines.Add(new[] { 256 + 200 * Math.Cos(f), 256 + 200 * Math.Sin(f), 256 + 200 * Math.Cos(f+step), 256 + 200 * Math.Sin(f+step),0,0,0,1 });

				lines.Add(new[] { 256, 256, 256 + 180 * Math.Cos(count / 100.0 * 2 * Math.PI), 256 + 180 * Math.Sin(count / 100.0 * 2 * Math.PI), 1,0,0,1 });


				Connection.sendTunnel("scene/panel/drawlines", new
				{
					id = panelId,
					width = 1,
					lines = lines.ToArray()
				});
				Connection.sendTunnel("scene/panel/swap", new
				{
					id = panelId,
				});

				count++;
				if (count == 1000)
					t.Stop();
			});

			t.Interval = 30;
			t.Start();
		}

		private void button3_Click(object sender, EventArgs e)
		{
			Connection.sendTunnel("scene/reset", null);

			string panelId = Connection.sendTunnelWait("scene/node/add",
			new
			{
				name = "panel",
				components = new
				{
					transform = new
					{
						position = new[] { 0, 1, 0 },
						rotation = new[] { 0, 0, 0 },
						scale = 1
					},
					panel = new
					{
						size = new[] { 1, 1 },
						resolution = new[] { 512, 512 },
					}
				}
			}, data => data.uuid);

			Connection.sendTunnel("scene/node/moveto", new
			{
				id = panelId,
				position = new[] { 0, 1, -50 },
				speed = 0.1
			});


			Connection.sendTunnel("scene/panel/clear", new
			{
				id = panelId,
			});

			Connection.sendTunnel("scene/panel/drawtext", new
			{
				id = panelId,
				text = "Hello world",
				position = new[] { 100, 100 }
			});
			Connection.sendTunnel("scene/panel/swap", new
			{
				id = panelId,
			});
		}

		private void button4_Click(object sender, EventArgs e)
		{
			Connection.sendTunnel("scene/reset", null);
			string[] map = new string[10]
			{
				"          ",
				" #----T-# ",
				" |hh  | | ",
				" T---T+-T ",
				" |   |  | ",
				" |h#-#  | ",
				" |h|    | ",
				" |h|    | ",
				" #-T----# ",
				"          "
			};

			Func<char, bool> isEmpty = c => c == ' ' || c == 'h';


			for(int x = 0; x < 10; x++)
			{
				for(int y = 0; y < 10; y++)
				{
					float scale = 10;
					string file = "";
					float rotation = 0;
					if (map[y][x] == '-' || map[y][x] == '|')
						file = "data/NetworkEngine/models/roads/set1/Road-2-Lane-Straight.obj";
					if (map[y][x] == '|')
						rotation = 90;

					if (map[y][x] == 'T')
					{
						file = "data/NetworkEngine/models/roads/set1/Road-2-Lane-T.obj";
						if (isEmpty(map[y][x + 1]))
							rotation = 90;
						else if (isEmpty(map[y][x - 1]))
							rotation = 270;
						else if (isEmpty(map[y - 1][x]))
							rotation = 0;
						else if (isEmpty(map[y + 1][x]))
							rotation = 180;
						else
							file = "";
					}
					if (map[y][x] == '#')
					{
						file = "data/NetworkEngine/models/roads/set1/Road-2-Lane-Corner.obj";
						if (isEmpty(map[y][x + 1]) && isEmpty(map[y + 1][x]))
							rotation = 0;
						else if (isEmpty(map[y][x + 1]) && isEmpty(map[y - 1][x]))
							rotation = 90;
						else if (isEmpty(map[y][x - 1]) && isEmpty(map[y + 1][x]))
							rotation = 270;
						else if (isEmpty(map[y][x - 1]) && isEmpty(map[y - 1][x]))
							rotation = 180;
					}
					if (map[y][x] == '+')
					{
						file = "data/NetworkEngine/models/roads/set1/Road-2-Lane-X.obj";
					}

					if(map[y][x] == 'h')
					{
						file = "data/NetworkEngine/models/houses/set1/house15.obj";
						scale = 4;
						rotation = 90;
					}

					if (file == "")
						continue;
					Connection.sendTunnel("scene/node/add",
					new
					{
						name = "road",
						components = new
						{
							transform = new
							{
								position = new[] { -50 + 10*x, 0, -100 + 10*y },
								rotation = new[] { 0, rotation, 0 },
								scale = scale
							},
							model = new
							{
								file = file
							}
						}
					});
				}
			}



			Connection.sendTunnel("setcallback",
			new
			{
				type = "button",
				button = "trigger",
				hand = "right"
			});


			bool moving = false;


			Connection.callbacks.Add("callback", (data) =>
			{
				if (data.state == "on")
					moving = true;
				if (data.state == "off")
				{
					moving = false;
				}
			});


			var t = new System.Windows.Forms.Timer(); ;
			t.Tick += (a, b) =>
			{
				if(moving)
				{
					dynamic data = Connection.sendTunnelWait("get",
					new
					{
						type = "handleft"
					}, d => d);

					var camera = Connection.sendTunnelWait("scene/node/find", new { name = "Camera" }, d => d[0]);

					float[] positioncamera = new float[3];
					positioncamera[0] = camera.components[0].position[0];
					positioncamera[1] = camera.components[0].position[1];
					positioncamera[2] = camera.components[0].position[2];

					positioncamera[0] += (float)data.forward[0] * 2;
					positioncamera[1] += (float)data.forward[1] * 2;
					positioncamera[2] += (float)data.forward[2] * 2;

					Connection.sendTunnel("scene/node/moveto",
					new
					{
						id = camera.uuid,
						position = positioncamera,
						speed = 3,
						rotate = "none",
						followheight = false,
						interpolate = "linear"
					});

				}
			};
			t.Interval = 1;
			t.Start();
			Connection.sendTunnel("scene/skybox/settime", new { time = 13 });
		}

		private void button5_Click(object sender, EventArgs e)
		{
			Connection.sendTunnel("scene/reset", null);
			Connection.sendTunnel("scene/node/add",
			new
			{
				name = "car",
				components = new
				{
					transform = new
					{
						position = new[] { 0, 0, 0 },
						rotation = new[] { 0, 0, 0 },
						scale = 0.025
					},
					model = new
					{
						file = "data/NetworkEngine/models/cars/white/car_white.obj"
					}
				}
			});
		}

		private void button6_Click(object sender, EventArgs e)
		{
			Connection.sendTunnel("scene/reset", null);
			Connection.sendTunnel("scene/skybox/update",
			new
			{
				type = "static",
				files = new 
				{
					xpos = "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_rt.png",
					xneg = "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_lf.png",
					ypos = "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_up.png",
					yneg = "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_dn.png",
					zpos = "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_bk.png",
					zneg = "data/NetworkEngine/textures/SkyBoxes/interstellar/interstellar_ft.png"
				}
			});
		}

		private void button7_Click(object sender, EventArgs e)
		{
			Connection.sendTunnel("scene/reset", null);


			string headId = Connection.sendTunnelWait("scene/node/find",
				new { name = "Head" }, data => data[0].uuid);
			Console.WriteLine("Head ID: " + headId);

			string cameraId = Connection.sendTunnelWait("scene/node/find",
				new { name = "Camera" }, data => data[0].uuid);
			Console.WriteLine("cameraId ID: " + cameraId);

			string panelId = Connection.sendTunnelWait("scene/node/add",
			new
			{
				name = "panel",
				parent = headId,
				components = new
				{
					transform = new
					{
						position = new[] { 0, 0, -1 },
						rotation = new[] { 0, 0, 0 },
						scale = 1
					},
					panel = new
					{
						size = new[] { 1, 1 },
						resolution = new[] { 512, 512 },
						background = new[] { 1, 1, 1, 0.25f }
					}
				}
			}, data => data.uuid);

			string carId = Connection.sendTunnelWait("scene/node/add",
			new
			{
				name = "car",
				components = new
				{
					transform = new
					{
						position = new[] { 0, 0, 0 },
						rotation = new[] { 0, 0, 0 },
						scale = 0.025
					},
					model = new
					{
						file = "data/NetworkEngine/models/cars/white/car_white.obj"
					}
				}
			}, data => data.uuid);

			Connection.sendTunnel("scene/node/update",
				new
				{
					id = cameraId,
					parent = carId,
					transform = new
					{
						scale = 1/ 0.025
					}
				});



			/*Connection.sendTunnel("scene/node/moveto", new
			{
				id = cameraId,
				position = new[] { 0, 0, -50 },
				speed = 0.5
			});*/

			float roundness = 10;
			float size = 20;
			string routeId = Connection.sendTunnelWait("route/add", new
			{
				nodes = new[]
				{
						new {   pos = new [] { size, 5, -size },    dir = new [] { roundness, 0, roundness } },
						new {   pos = new [] { size, 0, size },     dir = new [] {-roundness, 0, roundness } },
						new {   pos = new [] { -size, 5, size },    dir = new [] {-roundness, 0,-roundness } },
						new {   pos = new [] { -size, 0, -size },   dir = new [] { roundness, 0,-roundness } }
					}

			}, data => data.uuid);


			Connection.sendTunnel("route/follow", new
			{
				route = routeId,
				node = carId,
				speed = 2.0,
				rotate = "XYZ",
				followHeight = true
			});



		}
	}
}
