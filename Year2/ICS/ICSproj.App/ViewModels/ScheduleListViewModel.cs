using System.Collections.ObjectModel;
using System.Windows.Input;
using ICSproj.App.Commands;
using ICSproj.App.Extensions;
using ICSproj.App.Messages;
using ICSproj.App.Services;
using ICSproj.App.ViewModels.Interfaces;
using ICSproj.App.Wrappers;
using ICSproj.BL.Models;
using ICSproj.BL.Repositories;

namespace ICSproj.App.ViewModels
{
    public class ScheduleListViewModel : IScheduleListViewModel
    {
        private readonly IRepository<ScheduleDetailModel, ScheduleListModel> _scheduleRepository;
        private readonly IMediator _mediator;

        public ScheduleListViewModel(IRepository<ScheduleDetailModel, ScheduleListModel> scheduleRepository, IMediator mediator)
        {
            _scheduleRepository = scheduleRepository;
            _mediator = mediator;

            ScheduleSelectedCommand = new RelayCommand<ScheduleListModel>(ScheduleSelected);
            ScheduleNewCommand = new RelayCommand(ScheduleNew);

            mediator.Register<UpdateMessage<ScheduleWrapper>>(ScheduleUpdated);
            mediator.Register<DeleteMessage<ScheduleWrapper>>(ScheduleDeleted);
        }

        public ObservableCollection<ScheduleListModel> Schedule { get; set; } = new ObservableCollection<ScheduleListModel>();

        public ICommand ScheduleSelectedCommand { get; }
        public ICommand ScheduleNewCommand { get; }

        private void ScheduleNew() => _mediator.Send(new NewMessage<ScheduleWrapper>());

        private void ScheduleSelected(ScheduleListModel schedule) => _mediator.Send(new SelectedMessage<ScheduleWrapper> { Id = schedule.Id });

        private void ScheduleUpdated(UpdateMessage<ScheduleWrapper> _) => Load();

        private void ScheduleDeleted(DeleteMessage<ScheduleWrapper> _) => Load();

        public void Load()
        {
            Schedule.Clear();
            var schedule = _scheduleRepository.GetAll();
            Schedule.AddRange(schedule);
        }
    }
}
