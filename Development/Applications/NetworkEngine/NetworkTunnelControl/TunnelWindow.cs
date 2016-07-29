using System;
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
	}
}
