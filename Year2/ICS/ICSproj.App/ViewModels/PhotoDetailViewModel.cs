using System;
using System.Windows.Input;
using ICSproj.App.Commands;
using ICSproj.App.Messages;
using ICSproj.App.Services;
using ICSproj.App.ViewModels.Interfaces;
using ICSproj.App.Wrappers;
using ICSproj.BL.Models;
using ICSproj.BL.Repositories;

namespace ICSproj.App.ViewModels
{
    public class PhotoDetailViewModel : ViewModelBase, IPhotoDetailViewModel
    {
        private readonly IRepository<PhotoDetailModel, PhotoListModel> _photoRepository;
        private readonly IMediator _mediator;

        public PhotoDetailViewModel(IRepository<PhotoDetailModel, PhotoListModel> photoRepository, IMediator mediator)
        {
            _photoRepository = photoRepository;
            _mediator = mediator;

            SaveCommand = new RelayCommand(Save, CanSave);
            DeleteCommand = new RelayCommand(Delete);
        }

        public PhotoWrapper? Model { get; set; }
        public ICommand SaveCommand { get; set; }
        public ICommand DeleteCommand { get; set; }

        public void Load(Guid id)
        {
            Model = _photoRepository.GetById(id) ?? new PhotoDetailModel();
        }

        public void Save()
        {
            if (Model == null)
            {
                throw new InvalidOperationException("Null model cannot be saved !");
            }

            Model = _photoRepository.InsertOrUpdate(Model.Model);
            _mediator.Send(new UpdateMessage<PhotoWrapper> { Model = Model });
        }

        private bool CanSave() =>
            Model != null
            && Model.Photo.Length > 0;

        public void Delete()
        {
            if (Model == null)
            {
                throw new InvalidOperationException("Null model cannot be deleted !");
            }

            if (Model.Id != Guid.Empty)
            {
                if (!(_photoRepository.Delete(Model.Id)))
                {
                    throw new OperationCanceledException("Failed to delete model !");
                }

                _mediator.Send(new DeleteMessage<PhotoWrapper>
                {
                    Model = Model
                });
            }
        }

    }
}
