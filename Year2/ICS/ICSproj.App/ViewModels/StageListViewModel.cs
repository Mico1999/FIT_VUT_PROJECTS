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
    public class StageListViewModel : IStageListViewModel
    {
        private readonly IRepository<StageDetailModel, StageListModel> _stageRepository;
        private readonly IMediator _mediator;

        public StageListViewModel(IRepository<StageDetailModel, StageListModel> stageRepository, IMediator mediator)
        {
            _stageRepository = stageRepository;
            _mediator = mediator;

            StageSelectedCommand = new RelayCommand<StageListModel>(StageSelected);
            StageNewCommand = new RelayCommand(StageNew);

            mediator.Register<UpdateMessage<StageWrapper>>(StageUpdated);
            mediator.Register<DeleteMessage<StageWrapper>>(StageDeleted);
        }

        public ObservableCollection<StageListModel> Stages { get; set; } = new ObservableCollection<StageListModel>();

        public ICommand StageSelectedCommand { get; }
        public ICommand StageNewCommand { get; }

        private void StageNew() => _mediator.Send(new NewMessage<StageWrapper>());

        private void StageSelected(StageListModel stage) => _mediator.Send(new SelectedMessage<StageWrapper> { Id = stage.Id });

        private void StageUpdated(UpdateMessage<StageWrapper> _) => Load();

        private void StageDeleted(DeleteMessage<StageWrapper> _) => Load();

        public void Load()
        {
            Stages.Clear();
            var stages = _stageRepository.GetAll();
            Stages.AddRange(stages);
        }


    }
}
