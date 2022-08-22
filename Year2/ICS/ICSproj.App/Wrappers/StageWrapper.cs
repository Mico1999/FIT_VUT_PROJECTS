using System.Collections.ObjectModel;
using System.Linq;
using ICSproj.BL.Models;

namespace ICSproj.App.Wrappers
{
 
    public class StageWrapper : ModelWrapper<StageDetailModel>
    {
        public StageWrapper(StageDetailModel model)
            : base(model)
        {
            InitializePhotoDetailModelList(model);
            InitializeScheduleDetailModelList(model);
        }

        public string Name
        {
            get => GetValue<string>();
            set => SetValue(value);
        }

        public string Description
        {
            get => GetValue<string>();
            set => SetValue(value);
        }

        private void InitializePhotoDetailModelList(StageDetailModel model)
        {
            if (model.Photos == null)
            {
                return;
            }
            Photos = new ObservableCollection<PhotoWrapper>(
                model.Photos.Select(e => new PhotoWrapper(e)));

            RegisterCollection(Photos, model.Photos);
        }

        private void InitializeScheduleDetailModelList(StageDetailModel model)
        {
            if (model.Schedule == null)
            {
                return;
            }
            Schedule = new ObservableCollection<ScheduleWrapper>(
                model.Schedule.Select(e => new ScheduleWrapper(e)));

            RegisterCollection(Schedule, model.Schedule);
        }

        public ObservableCollection<PhotoWrapper> Photos { get; set; } = null!;
        public ObservableCollection<ScheduleWrapper> Schedule { get; set; } = null!;

        public static implicit operator StageWrapper(StageDetailModel detailModel)
            => new(detailModel);

        public static implicit operator StageDetailModel(StageWrapper wrapper)
            => wrapper.Model;
    }
}
