using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.IO;
using System.Windows;
using System.Drawing.Imaging;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using ICSproj.App.Commands;
using ICSproj.App.ViewModels;
using ICSproj.BL.Models;

namespace ICSproj.App.Views
{
    /// <summary>
    /// Interaction logic for StageDetailView.xaml
    /// </summary>
    public partial class StageDetailView
    {
        public StageDetailView()
        {
            InitializeComponent();
        }

        private void ButtonBase_OnClick(object sender, RoutedEventArgs e)
        {
            Stream myStream;
            OpenFileDialog thisDialog = new OpenFileDialog();

            thisDialog.InitialDirectory = "d:\\";
            thisDialog.Filter = "*.png|*.jpg";
            thisDialog.FilterIndex = 2;
            thisDialog.RestoreDirectory = true;
            thisDialog.Multiselect = true;
            thisDialog.Title = "Please Select Source File(s) for Conversion";

            if (thisDialog.ShowDialog() == true)
            {
                var viewModel = (StageDetailViewModel)DataContext;
                foreach (String file in thisDialog.FileNames)
                {
                    try
                    {
                        if ((myStream = thisDialog.OpenFile()) != null)
                        {
                            using (myStream)
                            {
                                Image img = Image.FromFile(file);
                                ImageSource source;
                                using (var ms = new MemoryStream())
                                {
                                    img.Save(ms, ImageFormat.Bmp);
                                    ms.Seek(0, SeekOrigin.Begin);

                                    var bitmapImage = new BitmapImage();
                                    bitmapImage.BeginInit();
                                    bitmapImage.CacheOption = BitmapCacheOption.OnLoad;
                                    bitmapImage.StreamSource = ms;
                                    bitmapImage.EndInit();

                                    source = bitmapImage;
                                }
                                byte[] arr;
                                using (MemoryStream ms = new MemoryStream())
                                {
                                    img.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                                    arr = ms.ToArray();
                                }
                                var model = new PhotoDetailModel()
                                {
                                    Photo = arr
                                };
                                StagePhoto.Source = source;
                                viewModel.Photo = model;
                            }
                        }
                    }

                    catch (Exception ex)
                    {
                        MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
                    }
                }
            }
        }
    }
}