using System.Collections.Generic;
using System.Linq;
using ICSproj.BL.Models;
using ICSproj.DAL.Entities;

namespace ICSproj.BL.Mappers
{
    public static class StageMapper
    {
        public static StageListModel MapStageEntityToListModel(StageEntity entity)
        {
            if (entity == null) return null;

            var modelSchedule = new List<ScheduleListModel>();
            if (entity.PerformanceMapping != null)
            {
                modelSchedule.AddRange(entity.PerformanceMapping.Select(item => ScheduleMapper.MapScheduleEntityToListModel(item)));
            }

            return new StageListModel
            {
                Id = entity.Id,
                Name = entity.Name,
                Description = entity.Description,
                Schedule = modelSchedule
            };
        }

        public static StageDetailModel MapStageEntityToDetailModel(StageEntity entity)
        {
            if (entity == null) return null;

            var modelPhotos = new List<PhotoDetailModel>();
            if (entity.Photos != null)
            {
                modelPhotos.AddRange(entity.Photos.Select(item => PhotoMapper.MapPhotoEntityToDetailModel(item)));
            }

            var modelSchedule = new List<ScheduleDetailModel>();
            if (entity.PerformanceMapping != null)
            {
                modelSchedule.AddRange(entity.PerformanceMapping.Select(item => ScheduleMapper.MapScheduleEntityToDetailModel(item)));
            }

            return new StageDetailModel
            {
                Id = entity.Id,
                Name = entity.Name,
                Description = entity.Description,
                Photos = modelPhotos,
                Schedule = modelSchedule
            };
        }

        public static StageEntity MapStageDetailModelToEntity(StageDetailModel model)
        {
            var entityPhotos = new List<PhotoEntity>();
            if (model.Photos != null)
            {
                entityPhotos.AddRange(model.Photos.Select(item => PhotoMapper.MapPhotoDetailModelToEntity(item)));
            }

            return new StageEntity
            {
                Id = model.Id,
                Name = model.Name,
                Description = model.Description,
                Photos = entityPhotos,
            };
        }
    }
}
