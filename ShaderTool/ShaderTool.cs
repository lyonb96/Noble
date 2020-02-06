using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.IO;

namespace ShaderTool
{
	public partial class ShaderTool : Form
	{
		private string VertShaderFile;
		private string FragShaderFile;
		private string VaryingDefFile;

		public ShaderTool()
		{
			InitializeComponent();
		}

		private void ShaderTool_Load(object sender, EventArgs e)
		{
			VertShaderFile = "";
			FragShaderFile = "";
			VaryingDefFile = "";
		}

		private void SelectVS_Click(object sender, EventArgs e)
		{
			if (OpenFileDialog.ShowDialog() == DialogResult.OK)
			{
				VertShaderFile = OpenFileDialog.FileName;
				VertShaderPath.Text = VertShaderFile;
			}
		}

		private void SelectFS_Click(object sender, EventArgs e)
		{
			if (OpenFileDialog.ShowDialog() == DialogResult.OK)
			{
				FragShaderFile = OpenFileDialog.FileName;
				FragShaderPath.Text = FragShaderFile;
			}
		}

		private void SelectVD_Click(object sender, EventArgs e)
		{
			if (OpenFileDialog.ShowDialog() == DialogResult.OK)
			{
				VaryingDefFile = OpenFileDialog.FileName;
				VaryingPath.Text = VaryingDefFile;
			}
		}

		private void CompileButton_Click(object sender, EventArgs e)
		{
			if (VertShaderFile.Length > 0 &&
				FragShaderFile.Length > 0 &&
				VaryingDefFile.Length > 0)
			{
				// Prompt to save file
				if (SaveFileDialog.ShowDialog() == DialogResult.OK)
				{
					// Store the final file to write
					string outFilePath = SaveFileDialog.FileName;

					string shadercArgs = "";
					// Compile Vertex Shader
					shadercArgs += "-f " + VertShaderFile; // Set file
					shadercArgs += " -o VSOutput.bin"; // Set output
					shadercArgs += " --varyingdef " + VaryingDefFile;
					shadercArgs += " --type v --platform windows -p vs_5_0 -i \"..\\Content\\shaders\"";

					System.Diagnostics.Process Proc = new System.Diagnostics.Process();
					Proc.StartInfo.FileName = "shaderc.exe";
					Proc.StartInfo.Arguments = shadercArgs;
					Proc.StartInfo.UseShellExecute = false;
					Proc.StartInfo.RedirectStandardOutput = true;
					Proc.Start();

					// Read the output
					string vsOutput = Proc.StandardOutput.ReadToEnd();
					Proc.WaitForExit();

					if (vsOutput.Length > 0)
					{
						// Failed to compile, show message
						string outText = "Compile Failed, shaderc message:\n" + vsOutput;
						OutputBox.Text = outText;
						return;
					}

					// Now compile frag shader
					shadercArgs = "";
					shadercArgs += "-f " + FragShaderFile; // Set file
					shadercArgs += " -o FSOutput.bin"; // Set output
					shadercArgs += " --varyingdef " + VaryingDefFile;
					shadercArgs += " --type f --platform windows -p ps_5_0 -i \"..\\Content\\shaders\"";

					Proc.StartInfo.Arguments = shadercArgs;
					Proc.Start();

					string fsOutput = Proc.StandardOutput.ReadToEnd();
					Proc.WaitForExit();
					if (fsOutput.Length > 0)
					{
						// Failed to compile, show message
						string outText = "Compile Failed, shaderc message:\n" + fsOutput;
						OutputBox.Text = outText;
						return;
					}

					// After compiling both, build the final output file
					// This conglomerates both into a single file
					BinaryReader vsBin = new BinaryReader(File.Open("VSOutput.bin", FileMode.Open));
					FileInfo vsInfo = new FileInfo("VSOutput.bin");
					BinaryReader fsBin = new BinaryReader(File.Open("FSOutput.bin", FileMode.Open));
					FileInfo fsInfo = new FileInfo("FSOutput.bin");

					BinaryWriter finalOutput = new BinaryWriter(File.Open(outFilePath, FileMode.Truncate));

					// Write VS
					finalOutput.Write((uint)vsInfo.Length);
					finalOutput.Write(vsBin.ReadBytes((int)vsInfo.Length));

					// Write FS
					finalOutput.Write((uint)fsInfo.Length);
					finalOutput.Write(fsBin.ReadBytes((int)fsInfo.Length));

					// Close resources
					vsBin.Close();
					fsBin.Close();
					finalOutput.Close();

					OutputBox.Text = "Compilation successful";
				}
			}
		}

		private void VertShaderPath_TextChanged(object sender, EventArgs e)
		{

		}
	}
}
