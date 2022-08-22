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
    public class BandListViewModel : IBandListViewModel
    {
        private readonly IRepository<BandDetailModel, BandListModel> _bandRepository;
        private readonly IMediator _mediator;

        public BandListViewModel(IRepository<BandDetailModel, BandListModel> bandRepository, IMediator mediator)
        {
            _bandRepository = bandRepository;
            _mediator = mediator;

            BandSelectedCommand = new RelayCommand<BandListModel>(BandSelected);
            BandNewCommand = new RelayCommand(BandNew);

            mediator.Register<UpdateMessage<BandWrapper>>(BandUpdated);
            mediator.Register<DeleteMessage<BandWrapper>>(BandDeleted);
        }

        public ObservableCollection<BandListModel> Bands { get; set; } = new ObservableCollection<BandListModel>();

        public ICommand BandSelectedCommand { get; }
        public ICommand BandNewCommand { get; }

        private void BandNew() => _mediator.Send(new NewMessage<BandWrapper>());

        private void BandSelected(BandListModel band) => _mediator.Send(new SelectedMessage<BandWrapper> { Id = band.Id });

        private void BandUpdated(UpdateMessage<BandWrapper> _) => Load();

        private void BandDeleted(DeleteMessage<BandWrapper> _) => Load();

        public void Load()
        {
            Bands.Clear();
            var bands = _bandRepository.GetAll();
            Bands.AddRange(bands);
        }

    }
}
