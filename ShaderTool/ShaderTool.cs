using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using Assimp;
using Assimp.Configs;

using System.IO;

namespace ShaderTool
{
	public partial class ShaderTool : Form
	{
		private string VertShaderFile;
		private string FragShaderFile;
		private string VaryingDefFile;

		private string AttributeName;
		private int SelectedAttributeType;
		private int AttributeCount;

		struct ShaderAttribute
		{
			public string AttributeName;
			public int AttributeType;
			public int AttributeCount;
		}

		private List<ShaderAttribute> Attributes;

		public ShaderTool()
		{
			InitializeComponent();
		}

		private void ShaderTool_Load(object sender, EventArgs e)
		{
			VertShaderFile = "";
			FragShaderFile = "";
			VaryingDefFile = "";
			SelectedAttributeType = 0;
			Attributes = new List<ShaderAttribute>();
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

					FileMode mode;
					if (File.Exists(outFilePath))
					{
						mode = FileMode.Truncate;
					}
					else
					{
						mode = FileMode.Create;
					}

					BinaryWriter finalOutput = new BinaryWriter(File.Open(outFilePath, mode));

					// Write attributes
					finalOutput.Write(Attributes.Count);
					foreach (ShaderAttribute attr in Attributes)
					{
						finalOutput.Write(attr.AttributeName);
						finalOutput.Write((uint)attr.AttributeType);
						finalOutput.Write((uint)attr.AttributeCount);
					}

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

					// Clear input states
					VertShaderFile = "";
					VertShaderPath.Text = "";
					FragShaderFile = "";
					FragShaderPath.Text = "";
					VaryingDefFile = "";
					VaryingPath.Text = "";

					Attributes.Clear();
					AttributeList.Items.Clear();
				}
			}
		}

		private void VertShaderPath_TextChanged(object sender, EventArgs e)
		{

		}

		/**
		 * Attribute designation
		 */

		private void AttributeList_SelectedIndexChanged(object sender, EventArgs e)
		{
		}

		private void DeleteAttribute_Click(object sender, EventArgs e)
		{
			int index = AttributeList.SelectedIndex;
			AttributeList.Items.RemoveAt(index);
			Attributes.RemoveAt(index);
		}

		private void AddAttribute_Click(object sender, EventArgs e)
		{
			if (TypeComboBox.SelectedItem != null && AttributeCount != 0 && AttributeName.Length > 0)
			{
				ShaderAttribute attr;
				attr.AttributeName = AttributeName;
				attr.AttributeType = SelectedAttributeType;
				attr.AttributeCount = AttributeCount;

				Attributes.Add(attr);

				// Add to the attribute list
				AttributeList.Items.Add(AttributeName);

				// Reset params
				AttributeNameBox.Text = "";
				TypeComboBox.SelectedItem = null;
				AttributeCountBox.Text = "";
			}
		}

		private void AttributeNameBox_TextChanged(object sender, EventArgs e)
		{
			AttributeName = AttributeNameBox.Text;
		}

		private void TypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			SelectedAttributeType = TypeComboBox.SelectedIndex;
		}

		private void AttributeCountBox_TextChanged(object sender, EventArgs e)
		{
			if (AttributeCountBox.TextLength == 0)
			{
				return;
			}
			try
			{
				AttributeCount = int.Parse(AttributeCountBox.Text);
			}
			catch (Exception)
			{
				AttributeCount = 0;
				OutputBox.Text = "Attribute Count must be an integer value";
			}

			if (AttributeCount > 4 || AttributeCount < 1)
			{
				OutputBox.Text = "Attribute Count cannot be greater than 4 or less than 1";
				AttributeCount = 0;
			}
		}

		/**
		 * Mesh conversion
		 */

		private string SelectedMeshFile = "";

		private void MeshSelectButton_Click(object sender, EventArgs e)
		{
			OpenFileDialog.Filter = "Supported Mesh Files (OBJ, 3DS)|*.OBJ;*.3DS";
			if (OpenFileDialog.ShowDialog() == DialogResult.OK)
			{
				SelectedMeshFile = OpenFileDialog.FileName;
				MeshBox.Text = SelectedMeshFile;
			}
			OpenFileDialog.Filter = "";
		}

		private void ConvertMeshButton_Click(object sender, EventArgs e)
		{
			if (SelectedMeshFile.Length > 0)
			{
				// Prompt user to save mesh to a file
				if (SaveFileDialog.ShowDialog() == DialogResult.OK)
				{
					string resultFile = SaveFileDialog.FileName;

					FileMode mode;
					if (File.Exists(resultFile))
					{
						mode = FileMode.Truncate;
					}
					else
					{
						mode = FileMode.Create;
					}

					BinaryWriter fileOut = new BinaryWriter(File.Open(resultFile, mode));

					// Load the mesh with Assimp.NET and convert to proprietary format
					AssimpContext importer = new AssimpContext();
					importer.SetConfig(new NormalSmoothingAngleConfig(66.0F));
					importer.SetConfig(new RemoveDegeneratePrimitivesConfig(true));

					PostProcessSteps post = 
						PostProcessSteps.CalculateTangentSpace |
						PostProcessSteps.Triangulate |
						PostProcessSteps.ImproveCacheLocality |
						PostProcessSteps.OptimizeMeshes |
						PostProcessSteps.OptimizeGraph |
						PostProcessSteps.FlipWindingOrder;
					Scene mesh = importer.ImportFile(SelectedMeshFile, post);

					if (mesh.HasMeshes)
					{
						Mesh m = mesh.Meshes[0];
						fileOut.Write((uint)m.VertexCount);
						for (int i = 0; i < m.VertexCount; ++i)
						{
							// Vertex pos
							fileOut.Write(m.Vertices[i].X);
							fileOut.Write(m.Vertices[i].Y);
							fileOut.Write(m.Vertices[i].Z);

							// Texture coordinates
							fileOut.Write(m.TextureCoordinateChannels[0][i].X);
							fileOut.Write(m.TextureCoordinateChannels[0][i].Y);

							// Normals
							fileOut.Write(m.Normals[i].X);
							fileOut.Write(m.Normals[i].Y);
							fileOut.Write(m.Normals[i].Z);

							// Tangents
							fileOut.Write(m.Tangents[i].X);
							fileOut.Write(m.Tangents[i].Y);
							fileOut.Write(m.Tangents[i].Z);
						}

						fileOut.Write((uint)m.GetUnsignedIndices().Length);
						for (int i = 0; i < m.GetUnsignedIndices().Length; ++i)
						{
							fileOut.Write((uint)m.GetUnsignedIndices()[i]);
						}

						fileOut.Close();
					}
				}
			}
		}
	}
}
