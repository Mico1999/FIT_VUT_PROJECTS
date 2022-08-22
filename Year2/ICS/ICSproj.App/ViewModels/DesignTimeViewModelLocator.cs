using ICSproj.App.Factories;
using ICSproj.App.Services;
using ICSproj.BL.Repositories;


namespace ICSproj.App.ViewModels
{
    public class DesignTimeViewModelLocator
    {
        private const string DesignTimeConnectionString = @"inMemory: TasksDB";

        public BandListViewModel BandListViewModel { get; }
        public StageListViewModel StageListViewModel { get; }
        public ScheduleListViewModel ScheduleListViewModel { get;}
        public PhotoListViewModel PhotoListViewModel { get; }
        public BandDetailViewModel BandDetailViewModel { get; set; }
        public StageDetailViewModel StageDetailViewModel { get; set; }
        public ScheduleDetailViewModel ScheduleDetailViewModel { get; set; }
        public PhotoDetailViewModel PhotoDetailViewModel { get; set; }


        public DesignTimeViewModelLocator()
        {
            var bandRepository = new BandRepository(new SqlServerDbContextFactory(DesignTimeConnectionString));
            var stageRepository = new StageRepository(new SqlServerDbContextFactory(DesignTimeConnectionString));
            var scheduleRepository = new ScheduleRepository(new SqlServerDbContextFactory(DesignTimeConnectionString));
            var photoRepository = new PhotoRepository(new SqlServerDbContextFactory(DesignTimeConnectionString));
            var mediator = new Mediator();
            var messageDialogService = new MessageDialogService();

            BandListViewModel = new BandListViewModel(bandRepository, mediator);
            StageListViewModel = new StageListViewModel(stageRepository, mediator);
            ScheduleListViewModel = new ScheduleListViewModel(scheduleRepository, mediator);
            PhotoListViewModel = new PhotoListViewModel(photoRepository, mediator);
            BandDetailViewModel = new BandDetailViewModel(bandRepository, photoRepository, mediator);
            StageDetailViewModel = new StageDetailViewModel(stageRepository, photoRepository, mediator);
            ScheduleDetailViewModel = new ScheduleDetailViewModel(scheduleRepository, bandRepository, stageRepository, mediator);
            PhotoDetailViewModel = new PhotoDetailViewModel(photoRepository, mediator);
        }
    }
}
