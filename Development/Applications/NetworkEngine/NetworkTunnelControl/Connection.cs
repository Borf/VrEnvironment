using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NetworkTunnelControl
{
	class Connection 
	{
		static TcpClient client;
		static NetworkStream stream;

		static byte[] totalBuffer = new byte[0];
		static byte[] buffer = new byte[1024];

		static string tunnelId = "";

		public delegate void Callback(dynamic json);

		public static Dictionary<string, Callback> callbacks = new Dictionary<string, Callback>();

		public static async Task connect()
		{
			client = new TcpClient();

			
			await client.ConnectAsync("borf.info", 6666);
			//await client.ConnectAsync("127.0.0.1", 6666);
			System.Console.WriteLine("Connected");
			stream = client.GetStream();
			stream.BeginRead(buffer, 0, 1024, onRead, null);

			callbacks["tunnel/send"] = (json) =>
			{
				string id = json.data.id;
				if (callbacks.ContainsKey(id))
				{
					/*try				{
						if (json["data"] != null && json["data"]["data"] != null && json["data"]["data"]["data"] != null && json["data"]["data"]["data"] is JObject && json.data.data.data.status == "error")
							Console.WriteLine("Got an error");
					}	catch (Exception e) { }*/
					Console.WriteLine("Got a packet " + id + " through tunnel");
					callbacks[id](json.data.data);
				}
				else
					Console.WriteLine("Got an unhandled packet " + id + " through tunnel");
			};
		}

		private static void onRead(IAsyncResult ar)
		{
			try
			{
				int rc = stream.EndRead(ar);
				totalBuffer = concat(totalBuffer, buffer, rc);
			}
			catch(System.IO.IOException)
			{
				Application.Exit();
				return;
			}
			while (totalBuffer.Length >= 4)
			{
				int packetSize = BitConverter.ToInt32(totalBuffer, 0);
				if (totalBuffer.Length >= packetSize + 4)
				{
					string data = Encoding.UTF8.GetString(totalBuffer, 4, packetSize);
					dynamic json = JsonConvert.DeserializeObject(data);
					Console.WriteLine("Got a packet " + json.id);
					
					string id = json.id;
					if (callbacks.ContainsKey(id))
						callbacks[id](json.data);

					totalBuffer = totalBuffer.SubArray(4 + packetSize, totalBuffer.Length - packetSize - 4);
				}
				else
					break;
			}
			stream.BeginRead(buffer, 0, 1024, onRead, null);
		}


		public static void send(string id, dynamic data)
		{
			var command = new { id = id, data = data };
			byte[] d = System.Text.Encoding.UTF8.GetBytes(JsonConvert.SerializeObject(command));
			stream.WriteAsync(BitConverter.GetBytes(d.Length), 0, 4).Wait();
			stream.WriteAsync(d, 0, d.Length).Wait();
		}

		public static List<Session> getSessions()
		{
			List<Session> sessions = new List<Session>();
			AutoResetEvent blocker = new AutoResetEvent(false);
			callbacks["session/list"] = (data) =>
			{
				foreach (var s in data)
					if(s.features.ToObject<List<string>>().Contains("tunnel"))
						sessions.Add(new Session() { id = s.id, ip = s.clientinfo.host, user = s.clientinfo.user, file = s.clientinfo.file });
				blocker.Set();
			};
			send("session/list", null);
			blocker.WaitOne(5000);
			callbacks.Remove("session/list");
			return sessions;
		}

		public static void openTunnel(string id, string key = "")
		{
			AutoResetEvent blocker = new AutoResetEvent(false);
			callbacks["tunnel/create"] = (data) =>
			{
				tunnelId = data.id;
				blocker.Set();
			};

			if(key != "")
				send("tunnel/create", new { session = id, key = key });
			else
				send("tunnel/create", new { session = id });
			blocker.WaitOne();
			Console.WriteLine("Tunnel ID is " + tunnelId);
			callbacks.Remove("tunnel/create");
		}

		public static void sendTunnel(string _id, dynamic _data)
		{
			var command = new { id = "tunnel/send", data = new { dest = tunnelId, data = new { id = _id, data = _data }  } };
			byte[] d = System.Text.Encoding.UTF8.GetBytes(JsonConvert.SerializeObject(command));
			stream.WriteAsync(BitConverter.GetBytes(d.Length), 0, 4).Wait();
			stream.WriteAsync(d, 0, d.Length).Wait();
			Thread.Sleep(50);
		}

		public delegate dynamic Callback2(dynamic json);
		public static dynamic sendTunnelWait(string _id, dynamic _data, Callback2 callback)
		{
			//for (int i = 0; i < 10; i++)
			{
				dynamic ret = null;
				AutoResetEvent blocker = new AutoResetEvent(false);
				callbacks[_id] = (d) =>
				{
					ret = callback(d);
					blocker.Set();
				};
				sendTunnel(_id, _data);
				if (blocker.WaitOne())
				{
					Connection.callbacks.Remove(_id);
					return ret;
				}
				Connection.callbacks.Remove(_id);
			}
			return null;
		}



		private static byte[] concat(byte[] b1, byte[] b2, int count)
		{
			byte[] r = new byte[b1.Length + count];
			System.Buffer.BlockCopy(b1, 0, r, 0, b1.Length);
			System.Buffer.BlockCopy(b2, 0, r, b1.Length, count);
			return r;
		}

	}

	public class Session
	{
		public string id;
		public string ip;
		public string user;
		public string file;
	}
}
