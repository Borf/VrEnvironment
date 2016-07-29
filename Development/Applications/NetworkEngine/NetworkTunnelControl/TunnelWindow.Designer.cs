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
			// TunnelWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1313, 653);
			this.Controls.Add(this.btnRemove);
			this.Controls.Add(this.btnPlant);
			this.Controls.Add(this.propertyGrid1);
			this.Controls.Add(this.treeView1);
			this.Name = "TunnelWindow";
			this.Text = "TunnelWindow";
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.TunnelWindow_FormClosed);
			this.Load += new System.EventHandler(this.TunnelWindow_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TreeView treeView1;
		private System.Windows.Forms.PropertyGrid propertyGrid1;
		private System.Windows.Forms.Button btnPlant;
		private System.Windows.Forms.Button btnRemove;
	}
}