namespace NetworkTunnelControl
{
	partial class TunnelWindow
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.treeView1 = new System.Windows.Forms.TreeView();
			this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
			this.btnPlant = new System.Windows.Forms.Button();
			this.btnRemove = new System.Windows.Forms.Button();
			this.timeOfDay = new System.Windows.Forms.TrackBar();
			this.lblTime = new System.Windows.Forms.Label();
			this.button1 = new System.Windows.Forms.Button();
			this.btnStuff = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.button3 = new System.Windows.Forms.Button();
			this.button4 = new System.Windows.Forms.Button();
			this.button5 = new System.Windows.Forms.Button();
			this.button6 = new System.Windows.Forms.Button();
			this.button7 = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.timeOfDay)).BeginInit();
			this.SuspendLayout();
			// 
			// treeView1
			// 
			this.treeView1.Location = new System.Drawing.Point(12, 12);
			this.treeView1.Name = "treeView1";
			this.treeView1.Size = new System.Drawing.Size(291, 306);
			this.treeView1.TabIndex = 0;
			this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
			// 
			// propertyGrid1
			// 
			this.propertyGrid1.Location = new System.Drawing.Point(12, 324);
			this.propertyGrid1.Name = "propertyGrid1";
			this.propertyGrid1.Size = new System.Drawing.Size(291, 317);
			this.propertyGrid1.TabIndex = 1;
			// 
			// btnPlant
			// 
			this.btnPlant.Location = new System.Drawing.Point(321, 12);
			this.btnPlant.Name = "btnPlant";
			this.btnPlant.Size = new System.Drawing.Size(159, 23);
			this.btnPlant.TabIndex = 2;
			this.btnPlant.Text = "Plant some trees";
			this.btnPlant.UseVisualStyleBackColor = true;
			this.btnPlant.Click += new System.EventHandler(this.button1_Click);
			// 
			// btnRemove
			// 
			this.btnRemove.Location = new System.Drawing.Point(321, 41);
			this.btnRemove.Name = "btnRemove";
			this.btnRemove.Size = new System.Drawing.Size(159, 23);
			this.btnRemove.TabIndex = 3;
			this.btnRemove.Text = "Remove selected";
			this.btnRemove.UseVisualStyleBackColor = true;
			this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
			// 
			// timeOfDay
			// 
			this.timeOfDay.LargeChange = 500;
			this.timeOfDay.Location = new System.Drawing.Point(1030, 12);
			this.timeOfDay.Maximum = 1000;
			this.timeOfDay.Name = "timeOfDay";
			this.timeOfDay.Size = new System.Drawing.Size(213, 45);
			this.timeOfDay.SmallChange = 100;
			this.timeOfDay.TabIndex = 4;
			this.timeOfDay.Scroll += new System.EventHandler(this.timeOfDay_Scroll);
			// 
			// lblTime
			// 
			this.lblTime.AutoSize = true;
			this.lblTime.Location = new System.Drawing.Point(941, 12);
			this.lblTime.Name = "lblTime";
			this.lblTime.Size = new System.Drawing.Size(30, 13);
			this.lblTime.TabIndex = 5;
			this.lblTime.Text = "Time";
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(321, 70);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(159, 23);
			this.button1.TabIndex = 6;
			this.button1.Text = "Terrrain";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click_1);
			// 
			// btnStuff
			// 
			this.btnStuff.Location = new System.Drawing.Point(649, 196);
			this.btnStuff.Name = "btnStuff";
			this.btnStuff.Size = new System.Drawing.Size(159, 23);
			this.btnStuff.TabIndex = 7;
			this.btnStuff.Text = "Do stuff";
			this.btnStuff.UseVisualStyleBackColor = true;
			this.btnStuff.Click += new System.EventHandler(this.btnStuff_Click);
			// 
			// button2
			// 
			this.button2.Location = new System.Drawing.Point(422, 424);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(159, 23);
			this.button2.TabIndex = 8;
			this.button2.Text = "PanelTest";
			this.button2.UseVisualStyleBackColor = true;
			this.button2.Click += new System.EventHandler(this.button2_Click);
			// 
			// button3
			// 
			this.button3.Location = new System.Drawing.Point(649, 424);
			this.button3.Name = "button3";
			this.button3.Size = new System.Drawing.Size(159, 23);
			this.button3.TabIndex = 9;
			this.button3.Text = "PanelText";
			this.button3.UseVisualStyleBackColor = true;
			this.button3.Click += new System.EventHandler(this.button3_Click);
			// 
			// button4
			// 
			this.button4.Location = new System.Drawing.Point(859, 424);
			this.button4.Name = "button4";
			this.button4.Size = new System.Drawing.Size(159, 23);
			this.button4.TabIndex = 10;
			this.button4.Text = "Roads";
			this.button4.UseVisualStyleBackColor = true;
			this.button4.Click += new System.EventHandler(this.button4_Click);
			// 
			// button5
			// 
			this.button5.Location = new System.Drawing.Point(321, 99);
			this.button5.Name = "button5";
			this.button5.Size = new System.Drawing.Size(159, 23);
			this.button5.TabIndex = 11;
			this.button5.Text = "ModelTest";
			this.button5.UseVisualStyleBackColor = true;
			this.button5.Click += new System.EventHandler(this.button5_Click);
			// 
			// button6
			// 
			this.button6.Location = new System.Drawing.Point(321, 128);
			this.button6.Name = "button6";
			this.button6.Size = new System.Drawing.Size(159, 23);
			this.button6.TabIndex = 12;
			this.button6.Text = "Skybox";
			this.button6.UseVisualStyleBackColor = true;
			this.button6.Click += new System.EventHandler(this.button6_Click);
			// 
			// button7
			// 
			this.button7.Location = new System.Drawing.Point(422, 496);
			this.button7.Name = "button7";
			this.button7.Size = new System.Drawing.Size(159, 23);
			this.button7.TabIndex = 13;
			this.button7.Text = "Panel met head";
			this.button7.UseVisualStyleBackColor = true;
			this.button7.Click += new System.EventHandler(this.button7_Click);
			// 
			// TunnelWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1313, 653);
			this.Controls.Add(this.button7);
			this.Controls.Add(this.button6);
			this.Controls.Add(this.button5);
			this.Controls.Add(this.button4);
			this.Controls.Add(this.button3);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.btnStuff);
			this.Controls.Add(this.button1);
			this.Controls.Add(this.lblTime);
			this.Controls.Add(this.timeOfDay);
			this.Controls.Add(this.btnRemove);
			this.Controls.Add(this.btnPlant);
			this.Controls.Add(this.propertyGrid1);
			this.Controls.Add(this.treeView1);
			this.Name = "TunnelWindow";
			this.Text = "TunnelWindow";
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.TunnelWindow_FormClosed);
			this.Load += new System.EventHandler(this.TunnelWindow_Load);
			((System.ComponentModel.ISupportInitialize)(this.timeOfDay)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TreeView treeView1;
		private System.Windows.Forms.PropertyGrid propertyGrid1;
		private System.Windows.Forms.Button btnPlant;
		private System.Windows.Forms.Button btnRemove;
		private System.Windows.Forms.TrackBar timeOfDay;
		private System.Windows.Forms.Label lblTime;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Button btnStuff;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.Button button3;
		private System.Windows.Forms.Button button4;
		private System.Windows.Forms.Button button5;
		private System.Windows.Forms.Button button6;
		private System.Windows.Forms.Button button7;
	}
}