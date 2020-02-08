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
			this.AttributeBox = new System.Windows.Forms.GroupBox();
			this.AttributeCountBox = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.TypeComboBox = new System.Windows.Forms.ComboBox();
			this.label2 = new System.Windows.Forms.Label();
			this.AttributeNameBox = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.DeleteAttribute = new System.Windows.Forms.Button();
			this.AddAttribute = new System.Windows.Forms.Button();
			this.AttributeList = new System.Windows.Forms.ListBox();
			this.ToolTab = new System.Windows.Forms.TabControl();
			this.ShaderToolPage = new System.Windows.Forms.TabPage();
			this.MeshToolPage = new System.Windows.Forms.TabPage();
			this.MeshSelectButton = new System.Windows.Forms.Button();
			this.MeshBox = new System.Windows.Forms.TextBox();
			this.ConvertMeshButton = new System.Windows.Forms.Button();
			this.OutputWindow.SuspendLayout();
			this.AttributeBox.SuspendLayout();
			this.ToolTab.SuspendLayout();
			this.ShaderToolPage.SuspendLayout();
			this.MeshToolPage.SuspendLayout();
			this.SuspendLayout();
			// 
			// SelectVS
			// 
			this.SelectVS.Location = new System.Drawing.Point(6, 6);
			this.SelectVS.Name = "SelectVS";
			this.SelectVS.Size = new System.Drawing.Size(153, 29);
			this.SelectVS.TabIndex = 0;
			this.SelectVS.Text = "Select Vertex Shader";
			this.SelectVS.UseVisualStyleBackColor = true;
			this.SelectVS.Click += new System.EventHandler(this.SelectVS_Click);
			// 
			// SelectFS
			// 
			this.SelectFS.Location = new System.Drawing.Point(6, 41);
			this.SelectFS.Name = "SelectFS";
			this.SelectFS.Size = new System.Drawing.Size(153, 29);
			this.SelectFS.TabIndex = 1;
			this.SelectFS.Text = "Select Frag Shader";
			this.SelectFS.UseVisualStyleBackColor = true;
			this.SelectFS.Click += new System.EventHandler(this.SelectFS_Click);
			// 
			// SelectVD
			// 
			this.SelectVD.Location = new System.Drawing.Point(6, 76);
			this.SelectVD.Name = "SelectVD";
			this.SelectVD.Size = new System.Drawing.Size(153, 29);
			this.SelectVD.TabIndex = 2;
			this.SelectVD.Text = "Select Varying Def";
			this.SelectVD.UseVisualStyleBackColor = true;
			this.SelectVD.Click += new System.EventHandler(this.SelectVD_Click);
			// 
			// CompileButton
			// 
			this.CompileButton.Location = new System.Drawing.Point(6, 111);
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
			this.OutputWindow.Location = new System.Drawing.Point(6, 146);
			this.OutputWindow.Name = "OutputWindow";
			this.OutputWindow.Size = new System.Drawing.Size(432, 263);
			this.OutputWindow.TabIndex = 6;
			this.OutputWindow.TabStop = false;
			this.OutputWindow.Text = "Output";
			// 
			// OutputBox
			// 
			this.OutputBox.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.OutputBox.Location = new System.Drawing.Point(7, 20);
			this.OutputBox.Multiline = true;
			this.OutputBox.Name = "OutputBox";
			this.OutputBox.ReadOnly = true;
			this.OutputBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.OutputBox.Size = new System.Drawing.Size(419, 237);
			this.OutputBox.TabIndex = 0;
			this.OutputBox.WordWrap = false;
			// 
			// VertShaderPath
			// 
			this.VertShaderPath.Location = new System.Drawing.Point(165, 11);
			this.VertShaderPath.Name = "VertShaderPath";
			this.VertShaderPath.ReadOnly = true;
			this.VertShaderPath.Size = new System.Drawing.Size(273, 20);
			this.VertShaderPath.TabIndex = 7;
			this.VertShaderPath.TextChanged += new System.EventHandler(this.VertShaderPath_TextChanged);
			// 
			// FragShaderPath
			// 
			this.FragShaderPath.Location = new System.Drawing.Point(165, 46);
			this.FragShaderPath.Name = "FragShaderPath";
			this.FragShaderPath.ReadOnly = true;
			this.FragShaderPath.Size = new System.Drawing.Size(273, 20);
			this.FragShaderPath.TabIndex = 8;
			// 
			// VaryingPath
			// 
			this.VaryingPath.Location = new System.Drawing.Point(165, 81);
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
			// AttributeBox
			// 
			this.AttributeBox.Controls.Add(this.AttributeCountBox);
			this.AttributeBox.Controls.Add(this.label3);
			this.AttributeBox.Controls.Add(this.TypeComboBox);
			this.AttributeBox.Controls.Add(this.label2);
			this.AttributeBox.Controls.Add(this.AttributeNameBox);
			this.AttributeBox.Controls.Add(this.label1);
			this.AttributeBox.Controls.Add(this.DeleteAttribute);
			this.AttributeBox.Controls.Add(this.AddAttribute);
			this.AttributeBox.Controls.Add(this.AttributeList);
			this.AttributeBox.Location = new System.Drawing.Point(449, 6);
			this.AttributeBox.Name = "AttributeBox";
			this.AttributeBox.Size = new System.Drawing.Size(262, 403);
			this.AttributeBox.TabIndex = 10;
			this.AttributeBox.TabStop = false;
			this.AttributeBox.Text = "Shader Attributes";
			// 
			// AttributeCountBox
			// 
			this.AttributeCountBox.Location = new System.Drawing.Point(7, 288);
			this.AttributeCountBox.Name = "AttributeCountBox";
			this.AttributeCountBox.Size = new System.Drawing.Size(249, 20);
			this.AttributeCountBox.TabIndex = 8;
			this.AttributeCountBox.TextChanged += new System.EventHandler(this.AttributeCountBox_TextChanged);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(7, 271);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(77, 13);
			this.label3.TabIndex = 7;
			this.label3.Text = "Attribute Count";
			// 
			// TypeComboBox
			// 
			this.TypeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.TypeComboBox.FormattingEnabled = true;
			this.TypeComboBox.Items.AddRange(new object[] {
            "Sampler",
            "Vec4",
            "Mat3",
            "Mat4"});
			this.TypeComboBox.Location = new System.Drawing.Point(7, 243);
			this.TypeComboBox.Name = "TypeComboBox";
			this.TypeComboBox.Size = new System.Drawing.Size(249, 21);
			this.TypeComboBox.TabIndex = 6;
			this.TypeComboBox.SelectedIndexChanged += new System.EventHandler(this.TypeComboBox_SelectedIndexChanged);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(7, 226);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(73, 13);
			this.label2.TabIndex = 5;
			this.label2.Text = "Attribute Type";
			// 
			// AttributeNameBox
			// 
			this.AttributeNameBox.Location = new System.Drawing.Point(7, 199);
			this.AttributeNameBox.Name = "AttributeNameBox";
			this.AttributeNameBox.Size = new System.Drawing.Size(249, 20);
			this.AttributeNameBox.TabIndex = 4;
			this.AttributeNameBox.TextChanged += new System.EventHandler(this.AttributeNameBox_TextChanged);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(7, 182);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(77, 13);
			this.label1.TabIndex = 3;
			this.label1.Text = "Attribute Name";
			// 
			// DeleteAttribute
			// 
			this.DeleteAttribute.Location = new System.Drawing.Point(7, 147);
			this.DeleteAttribute.Name = "DeleteAttribute";
			this.DeleteAttribute.Size = new System.Drawing.Size(104, 28);
			this.DeleteAttribute.TabIndex = 2;
			this.DeleteAttribute.Text = "Delete Attribute";
			this.DeleteAttribute.UseVisualStyleBackColor = true;
			this.DeleteAttribute.Click += new System.EventHandler(this.DeleteAttribute_Click);
			// 
			// AddAttribute
			// 
			this.AddAttribute.Location = new System.Drawing.Point(152, 147);
			this.AddAttribute.Name = "AddAttribute";
			this.AddAttribute.Size = new System.Drawing.Size(104, 28);
			this.AddAttribute.TabIndex = 1;
			this.AddAttribute.Text = "Add Attribute";
			this.AddAttribute.UseVisualStyleBackColor = true;
			this.AddAttribute.Click += new System.EventHandler(this.AddAttribute_Click);
			// 
			// AttributeList
			// 
			this.AttributeList.FormattingEnabled = true;
			this.AttributeList.Location = new System.Drawing.Point(7, 20);
			this.AttributeList.Name = "AttributeList";
			this.AttributeList.Size = new System.Drawing.Size(249, 121);
			this.AttributeList.TabIndex = 0;
			this.AttributeList.SelectedIndexChanged += new System.EventHandler(this.AttributeList_SelectedIndexChanged);
			// 
			// ToolTab
			// 
			this.ToolTab.Controls.Add(this.ShaderToolPage);
			this.ToolTab.Controls.Add(this.MeshToolPage);
			this.ToolTab.Location = new System.Drawing.Point(12, 12);
			this.ToolTab.Name = "ToolTab";
			this.ToolTab.SelectedIndex = 0;
			this.ToolTab.Size = new System.Drawing.Size(725, 446);
			this.ToolTab.TabIndex = 11;
			// 
			// ShaderToolPage
			// 
			this.ShaderToolPage.Controls.Add(this.SelectVS);
			this.ShaderToolPage.Controls.Add(this.AttributeBox);
			this.ShaderToolPage.Controls.Add(this.VertShaderPath);
			this.ShaderToolPage.Controls.Add(this.OutputWindow);
			this.ShaderToolPage.Controls.Add(this.VaryingPath);
			this.ShaderToolPage.Controls.Add(this.CompileButton);
			this.ShaderToolPage.Controls.Add(this.SelectFS);
			this.ShaderToolPage.Controls.Add(this.FragShaderPath);
			this.ShaderToolPage.Controls.Add(this.SelectVD);
			this.ShaderToolPage.Location = new System.Drawing.Point(4, 22);
			this.ShaderToolPage.Name = "ShaderToolPage";
			this.ShaderToolPage.Padding = new System.Windows.Forms.Padding(3);
			this.ShaderToolPage.Size = new System.Drawing.Size(717, 420);
			this.ShaderToolPage.TabIndex = 0;
			this.ShaderToolPage.Text = "Shader Tool";
			this.ShaderToolPage.UseVisualStyleBackColor = true;
			// 
			// MeshToolPage
			// 
			this.MeshToolPage.Controls.Add(this.ConvertMeshButton);
			this.MeshToolPage.Controls.Add(this.MeshBox);
			this.MeshToolPage.Controls.Add(this.MeshSelectButton);
			this.MeshToolPage.Location = new System.Drawing.Point(4, 22);
			this.MeshToolPage.Name = "MeshToolPage";
			this.MeshToolPage.Padding = new System.Windows.Forms.Padding(3);
			this.MeshToolPage.Size = new System.Drawing.Size(717, 420);
			this.MeshToolPage.TabIndex = 1;
			this.MeshToolPage.Text = "Mesh Tool";
			this.MeshToolPage.UseVisualStyleBackColor = true;
			// 
			// MeshSelectButton
			// 
			this.MeshSelectButton.Location = new System.Drawing.Point(7, 117);
			this.MeshSelectButton.Name = "MeshSelectButton";
			this.MeshSelectButton.Size = new System.Drawing.Size(153, 29);
			this.MeshSelectButton.TabIndex = 0;
			this.MeshSelectButton.Text = "Select Mesh";
			this.MeshSelectButton.UseVisualStyleBackColor = true;
			this.MeshSelectButton.Click += new System.EventHandler(this.MeshSelectButton_Click);
			// 
			// MeshBox
			// 
			this.MeshBox.Location = new System.Drawing.Point(166, 122);
			this.MeshBox.Name = "MeshBox";
			this.MeshBox.ReadOnly = true;
			this.MeshBox.Size = new System.Drawing.Size(545, 20);
			this.MeshBox.TabIndex = 8;
			// 
			// ConvertMeshButton
			// 
			this.ConvertMeshButton.Location = new System.Drawing.Point(286, 196);
			this.ConvertMeshButton.Name = "ConvertMeshButton";
			this.ConvertMeshButton.Size = new System.Drawing.Size(153, 29);
			this.ConvertMeshButton.TabIndex = 9;
			this.ConvertMeshButton.Text = "Convert Mesh";
			this.ConvertMeshButton.UseVisualStyleBackColor = true;
			this.ConvertMeshButton.Click += new System.EventHandler(this.ConvertMeshButton_Click);
			// 
			// ShaderTool
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(747, 469);
			this.Controls.Add(this.ToolTab);
			this.MaximizeBox = false;
			this.Name = "ShaderTool";
			this.Text = "Noble Shader Tool";
			this.Load += new System.EventHandler(this.ShaderTool_Load);
			this.OutputWindow.ResumeLayout(false);
			this.OutputWindow.PerformLayout();
			this.AttributeBox.ResumeLayout(false);
			this.AttributeBox.PerformLayout();
			this.ToolTab.ResumeLayout(false);
			this.ShaderToolPage.ResumeLayout(false);
			this.ShaderToolPage.PerformLayout();
			this.MeshToolPage.ResumeLayout(false);
			this.MeshToolPage.PerformLayout();
			this.ResumeLayout(false);

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
		private System.Windows.Forms.GroupBox AttributeBox;
		private System.Windows.Forms.Button DeleteAttribute;
		private System.Windows.Forms.Button AddAttribute;
		private System.Windows.Forms.ListBox AttributeList;
		private System.Windows.Forms.ComboBox TypeComboBox;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox AttributeNameBox;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox AttributeCountBox;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TabControl ToolTab;
		private System.Windows.Forms.TabPage ShaderToolPage;
		private System.Windows.Forms.TabPage MeshToolPage;
		private System.Windows.Forms.Button ConvertMeshButton;
		private System.Windows.Forms.TextBox MeshBox;
		private System.Windows.Forms.Button MeshSelectButton;
	}
}

