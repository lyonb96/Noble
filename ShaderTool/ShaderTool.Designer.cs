namespace ShaderTool
{
	partial class ShaderTool
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
			this.SelectVS = new System.Windows.Forms.Button();
			this.SelectFS = new System.Windows.Forms.Button();
			this.SelectVD = new System.Windows.Forms.Button();
			this.CompileButton = new System.Windows.Forms.Button();
			this.OutputWindow = new System.Windows.Forms.GroupBox();
			this.OutputBox = new System.Windows.Forms.TextBox();
			this.VertShaderPath = new System.Windows.Forms.TextBox();
			this.FragShaderPath = new System.Windows.Forms.TextBox();
			this.VaryingPath = new System.Windows.Forms.TextBox();
			this.OpenFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.SaveFileDialog = new System.Windows.Forms.SaveFileDialog();
			this.OutputWindow.SuspendLayout();
			this.SuspendLayout();
			// 
			// SelectVS
			// 
			this.SelectVS.Location = new System.Drawing.Point(12, 12);
			this.SelectVS.Name = "SelectVS";
			this.SelectVS.Size = new System.Drawing.Size(153, 29);
			this.SelectVS.TabIndex = 0;
			this.SelectVS.Text = "Select Vertex Shader";
			this.SelectVS.UseVisualStyleBackColor = true;
			this.SelectVS.Click += new System.EventHandler(this.SelectVS_Click);
			// 
			// SelectFS
			// 
			this.SelectFS.Location = new System.Drawing.Point(12, 47);
			this.SelectFS.Name = "SelectFS";
			this.SelectFS.Size = new System.Drawing.Size(153, 29);
			this.SelectFS.TabIndex = 1;
			this.SelectFS.Text = "Select Frag Shader";
			this.SelectFS.UseVisualStyleBackColor = true;
			this.SelectFS.Click += new System.EventHandler(this.SelectFS_Click);
			// 
			// SelectVD
			// 
			this.SelectVD.Location = new System.Drawing.Point(12, 82);
			this.SelectVD.Name = "SelectVD";
			this.SelectVD.Size = new System.Drawing.Size(153, 29);
			this.SelectVD.TabIndex = 2;
			this.SelectVD.Text = "Select Varying Def";
			this.SelectVD.UseVisualStyleBackColor = true;
			this.SelectVD.Click += new System.EventHandler(this.SelectVD_Click);
			// 
			// CompileButton
			// 
			this.CompileButton.Location = new System.Drawing.Point(12, 117);
			this.CompileButton.Name = "CompileButton";
			this.CompileButton.Size = new System.Drawing.Size(153, 29);
			this.CompileButton.TabIndex = 5;
			this.CompileButton.Text = "Compile Shaders";
			this.CompileButton.UseVisualStyleBackColor = true;
			this.CompileButton.Click += new System.EventHandler(this.CompileButton_Click);
			// 
			// OutputWindow
			// 
			this.OutputWindow.Controls.Add(this.OutputBox);
			this.OutputWindow.Location = new System.Drawing.Point(12, 152);
			this.OutputWindow.Name = "OutputWindow";
			this.OutputWindow.Size = new System.Drawing.Size(432, 263);
			this.OutputWindow.TabIndex = 6;
			this.OutputWindow.TabStop = false;
			this.OutputWindow.Text = "Output";
			// 
			// OutputBox
			// 
			this.OutputBox.Location = new System.Drawing.Point(7, 20);
			this.OutputBox.Multiline = true;
			this.OutputBox.Name = "OutputBox";
			this.OutputBox.ReadOnly = true;
			this.OutputBox.Size = new System.Drawing.Size(419, 237);
			this.OutputBox.TabIndex = 0;
			// 
			// VertShaderPath
			// 
			this.VertShaderPath.Location = new System.Drawing.Point(171, 17);
			this.VertShaderPath.Name = "VertShaderPath";
			this.VertShaderPath.ReadOnly = true;
			this.VertShaderPath.Size = new System.Drawing.Size(273, 20);
			this.VertShaderPath.TabIndex = 7;
			this.VertShaderPath.TextChanged += new System.EventHandler(this.VertShaderPath_TextChanged);
			// 
			// FragShaderPath
			// 
			this.FragShaderPath.Location = new System.Drawing.Point(171, 52);
			this.FragShaderPath.Name = "FragShaderPath";
			this.FragShaderPath.ReadOnly = true;
			this.FragShaderPath.Size = new System.Drawing.Size(273, 20);
			this.FragShaderPath.TabIndex = 8;
			// 
			// VaryingPath
			// 
			this.VaryingPath.Location = new System.Drawing.Point(171, 87);
			this.VaryingPath.Name = "VaryingPath";
			this.VaryingPath.ReadOnly = true;
			this.VaryingPath.Size = new System.Drawing.Size(273, 20);
			this.VaryingPath.TabIndex = 9;
			// 
			// OpenFileDialog
			// 
			this.OpenFileDialog.FileName = "OpenFile";
			this.OpenFileDialog.Title = "Select File";
			// 
			// SaveFileDialog
			// 
			this.SaveFileDialog.Title = "Output File Name";
			// 
			// ShaderTool
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(456, 427);
			this.Controls.Add(this.VaryingPath);
			this.Controls.Add(this.FragShaderPath);
			this.Controls.Add(this.VertShaderPath);
			this.Controls.Add(this.OutputWindow);
			this.Controls.Add(this.CompileButton);
			this.Controls.Add(this.SelectVD);
			this.Controls.Add(this.SelectFS);
			this.Controls.Add(this.SelectVS);
			this.MaximizeBox = false;
			this.Name = "ShaderTool";
			this.Text = "Noble Shader Tool";
			this.Load += new System.EventHandler(this.ShaderTool_Load);
			this.OutputWindow.ResumeLayout(false);
			this.OutputWindow.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button SelectVS;
		private System.Windows.Forms.Button SelectFS;
		private System.Windows.Forms.Button SelectVD;
		private System.Windows.Forms.Button CompileButton;
		private System.Windows.Forms.GroupBox OutputWindow;
		private System.Windows.Forms.TextBox OutputBox;
		private System.Windows.Forms.TextBox VertShaderPath;
		private System.Windows.Forms.TextBox FragShaderPath;
		private System.Windows.Forms.TextBox VaryingPath;
		private System.Windows.Forms.OpenFileDialog OpenFileDialog;
		private System.Windows.Forms.SaveFileDialog SaveFileDialog;
	}
}

