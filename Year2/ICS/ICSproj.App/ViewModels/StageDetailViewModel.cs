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
    public class StageDetailViewModel : ViewModelBase, IStageDetailViewModel
    {
        private readonly IRepository<StageDetailModel, StageListModel> _stageRepository;
        private readonly PhotoRepository _photoRepository;
        private readonly IMediator _mediator;

        public StageDetailViewModel(IRepository<StageDetailModel, StageListModel> stageRepository, IRepository<PhotoDetailModel, PhotoListModel> photoRepository, IMediator mediator)
        {
            _stageRepository = stageRepository;
            _photoRepository = (PhotoRepository)photoRepository;
            _mediator = mediator;

            SaveCommand = new RelayCommand(Save, CanSave);
            DeleteCommand = new RelayCommand(Delete);

            _mediator.Register<SelectedMessage<StageWrapper>>(StageSelectedPhoto);
        }

        private void StageSelectedPhoto(SelectedMessage<StageWrapper> message)
        {
            LoadPhotos(message.Id);
        }

        public void LoadPhotos(Guid id)
        {
            var photo = _photoRepository.GetByForeignId(id);
            Photo = photo;
        }

        public StageWrapper? Model { get; set; }
        public ICommand SaveCommand { get; set; }
        public ICommand DeleteCommand { get; set; }

        public PhotoDetailModel Photo { get; set; } = new PhotoDetailModel();

        public void Load(Guid id)
        {
            Model = _stageRepository.GetById(id) ?? new StageDetailModel();
        }

        public void Save()
        {
            if (Model == null)
            {
                throw new InvalidOperationException("Null model cannot be saved !");
            }

            Model = _stageRepository.InsertOrUpdate(Model.Model);
            _photoRepository.DeleteForeign(Model.Id);
            Photo.ForeignGuid = Model.Id;
            _photoRepository.InsertOrUpdate(Photo);

            _mediator.Send(new UpdateMessage<StageWrapper> { Model = Model });
        }

        private bool CanSave() =>
            Model != null
            && !string.IsNullOrWhiteSpace(Model.Name)
            && !string.IsNullOrWhiteSpace(Model.Description);

        public void Delete()
        {
            if (Model == null)
            {
                throw new InvalidOperationException("Null model cannot be deleted !");
            }

            if (Model.Id != Guid.Empty)
            {
                if (!(_stageRepository.Delete(Model.Id)))
                {
                    throw new OperationCanceledException("Failed to delete model !");
                }

                _mediator.Send(new DeleteMessage<StageWrapper>
                {
                    Model = Model
                });
            }
        }


    }
}
