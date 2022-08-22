using System;
using System.Collections.Generic;
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

namespace ICSproj.App
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        private ICollection<TabControl> ViewTabCtrlList { get; }


        public MainWindow(MainViewModel mainViewModel)
        {
            InitializeComponent();
            DataContext = mainViewModel;

            // Initialize static properties
            ViewTabCtrlList = new List<TabControl>
            {
                BandDetail,
                StageDetail,
                ScheduleDetail
            };
        }


        private void Menu_Bands_OnClick(object sender, RoutedEventArgs e)
        {
            if (BandListView.Visibility == Visibility.Visible) return;

            // Collapse all sibling views
            StageListView.Visibility = Visibility.Collapsed;
            ScheduleListView.Visibility = Visibility.Collapsed;

            // Collapse all siblings' tab controls and their possibly open childrenpages
            StageDetail.Visibility = Visibility.Collapsed;
            ScheduleDetail.Visibility = Visibility.Collapsed;
            
            // Show target
            BandListView.Visibility = Visibility.Visible;
            BandDetail.Visibility = Visibility.Visible;
        }


        private void Menu_Stages_OnClick(object sender, RoutedEventArgs e)
        {
            if (StageListView.Visibility == Visibility.Visible) return;

            // Collapse all sibling views
            BandListView.Visibility = Visibility.Collapsed;
            ScheduleListView.Visibility = Visibility.Collapsed;

            // Collapse all siblings' tab controls and their possibly open childrenpages
            BandDetail.Visibility = Visibility.Collapsed;
            ScheduleDetail.Visibility = Visibility.Collapsed;
            
            // Show target
            StageListView.Visibility = Visibility.Visible;
            StageDetail.Visibility = Visibility.Visible;
        }


        private void Menu_Schedules_OnClick(object sender, RoutedEventArgs e)
        {
            if (ScheduleListView.Visibility == Visibility.Visible) return;

            // Collapse all sibling views
            BandListView.Visibility = Visibility.Collapsed;
            StageListView.Visibility = Visibility.Collapsed;

            // Collapse all siblings' tab controls and their possibly open childrenpages
            BandDetail.Visibility = Visibility.Collapsed;
            StageDetail.Visibility = Visibility.Collapsed;

            // Show target
            ScheduleListView.Visibility = Visibility.Visible;
            ScheduleDetail.Visibility = Visibility.Visible;
        }
    }
}
