using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NetworkTunnelControl
{
	public partial class SessionSelect : Form
	{
		public SessionSelect()
		{
			InitializeComponent();
		}

		private async void SessionSelect_Load(object sender, EventArgs e)
		{
			await Connection.connect();

			button2_Click(null, null);

			for(int i = 0; i < tunnelApps.Items.Count; i++)
			{
				tunnelApps.Items[i].Selected = false;
				if (tunnelApps.Items[i].SubItems[2].Text == "cave")
				{
					tunnelApps.Items[i].Selected = true;
					button1_Click(null, null);
				}
			}

			//if (tunnelApps.Items.Count > 0)
			//	button1_Click(null, null);

			Timer t = new Timer();
			t.Tick += new EventHandler((a, b) => reload());
			t.Interval = 5000;
			t.Start();

		}

		public void button2_Click(object sender, EventArgs e)
		{
			reload();
		}

		public void reload()
		{
			tunnelApps.FullRowSelect = true;
			tunnelApps.GridLines = true;
			tunnelApps.Columns.Clear();
			tunnelApps.Columns.Add("ID");
			tunnelApps.Columns.Add("Host");
			tunnelApps.Columns.Add("user");
			tunnelApps.Columns.Add("Filename");

			tunnelApps.Items.Clear();
			var sessions = Connection.getSessions();
			foreach (var session in sessions)
				tunnelApps.Items.Add(session.id).SubItems.AddRange(new string[] { session.ip, session.user, session.file });

			tunnelApps.SelectedIndices.Clear();
			if (tunnelApps.Items.Count > 0)
				tunnelApps.SelectedIndices.Add(0);

			tunnelApps.AutoResizeColumns(ColumnHeaderAutoResizeStyle.ColumnContent);

			for (int i = 0; i < tunnelApps.Items.Count; i++)
			{
				tunnelApps.Items[i].Selected = false;
				if (tunnelApps.Items[i].SubItems[2].Text == "CAVE")
				{
					tunnelApps.Items[i].Selected = true;
					button1_Click(null, null);
				}
			}
		}

		private void button1_Click(object sender, EventArgs e)
		{
			if (tunnelApps.SelectedItems.Count != 1)
				return;
			if (!Visible)
				return;
			string id = tunnelApps.SelectedItems[0].Text;
			Console.WriteLine("Tunneling to " + id);
			Connection.openTunnel(id, "muffins");
			Hide();
			new TunnelWindow(this).Show();
		}
	}
}
