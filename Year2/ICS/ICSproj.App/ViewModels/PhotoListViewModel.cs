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

    public class PhotoListViewModel : IPhotoListViewModel
    {
        private readonly IRepository<PhotoDetailModel, PhotoListModel> _photoRepository;
        private readonly IMediator _mediator;

        public PhotoListViewModel(IRepository<PhotoDetailModel, PhotoListModel> photoRepository, IMediator mediator)
        {
            _photoRepository = photoRepository;
            _mediator = mediator;

            PhotoSelectedCommand = new RelayCommand<PhotoListModel>(PhotoSelected);
            PhotoNewCommand = new RelayCommand(PhotoNew);

            mediator.Register<UpdateMessage<PhotoWrapper>>(PhotoUpdated);
            mediator.Register<DeleteMessage<PhotoWrapper>>(PhotoDeleted);
        }

        public ObservableCollection<PhotoListModel> Photos { get; set; } = new ObservableCollection<PhotoListModel>();

        public ICommand PhotoSelectedCommand { get; }
        public ICommand PhotoNewCommand { get; }

        private void PhotoNew() => _mediator.Send(new NewMessage<PhotoWrapper>());

        private void PhotoSelected(PhotoListModel photo) => _mediator.Send(new SelectedMessage<PhotoWrapper> { Id = photo.Id });

        private void PhotoUpdated(UpdateMessage<PhotoWrapper> _) => Load();

        private void PhotoDeleted(DeleteMessage<PhotoWrapper> _) => Load();

        public void Load()
        {
            Photos.Clear();
            var photos = _photoRepository.GetAll();
            Photos.AddRange(photos);
        }

    }
}