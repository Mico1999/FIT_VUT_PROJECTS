using System.Collections.Generic;
using System.Linq;
using ICSproj.BL.Models;
using ICSproj.DAL.Entities;

namespace ICSproj.BL.Mappers
{
    public static class BandMapper
    {
        public static BandListModel MapBandEntityToListModel(BandEntity entity)
        {
            if (entity == null) return null;

            var modelPhotos = new List<PhotoListModel>();
            if (entity.Photos != null)
            {
                modelPhotos.AddRange(entity.Photos.Select(item => PhotoMapper.MapPhotoEntityToListModel(item)));
            }

            var modelSchedule = new List<ScheduleListModel>();
            if (entity.PerformanceMapping != null)
            {
                modelSchedule.AddRange(entity.PerformanceMapping.Select(item => ScheduleMapper.MapScheduleEntityToListModel(item)));
            }

            return new BandListModel
            {
                Id = entity.Id,
                Name = entity.Name,
                Description = entity.Description,
                Photos = modelPhotos,
                Schedule = modelSchedule
            };
        }

        public static BandDetailModel MapBandEntityToDetailModel(BandEntity entity)
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

            return new BandDetailModel
            {
                Id = entity.Id,
                Name = entity.Name,
                Description = entity.Description,
                Photos = modelPhotos,
                Schedule = modelSchedule,
                Genre = entity.Genre,
                OriginCountry = entity.OriginCountry,
                DescriptionLong = entity.DescriptionLong
            };
        }

        public static BandEntity MapBandDetailModelToEntity(BandDetailModel model)
        {
            var entityPhotos = new List<PhotoEntity>();
            if (model.Photos != null)
            {
                entityPhotos.AddRange(model.Photos.Select(item => PhotoMapper.MapPhotoDetailModelToEntity(item)));
            }

            return new BandEntity
            {
                Id = model.Id,
                Name = model.Name,
                Description = model.Description,
                Photos = entityPhotos,
                Genre = model.Genre,
                OriginCountry = model.OriginCountry,
                DescriptionLong = model.DescriptionLong
            };
        }
    }
}
