using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ICSproj.App.ViewModels;
using ICSproj.BL.Models;
using Microsoft.Win32;
using Image = System.Drawing.Image;

namespace ICSproj.App.Views
{
    /// <summary>
    /// Interaction logic for BandDetailView.xaml
    /// </summary>
    public partial class BandDetailView
    {
        public BandDetailView()
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
                var viewModel = (BandDetailViewModel)DataContext;
                foreach (String file in thisDialog.FileNames)
                {
                    try
                    {
                        if ((myStream = thisDialog.OpenFile()) != null)
                        {
                            using (myStream)
                            {
                                System.Drawing.Image img = Image.FromFile(file);
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
                                BandPhoto.Source = source;
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
