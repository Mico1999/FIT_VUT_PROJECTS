using ICSproj.BL.Models;
using ICSproj.DAL.Entities;

namespace ICSproj.BL.Mappers
{
    public static class PhotoMapper
    {
        public static PhotoListModel MapPhotoEntityToListModel(PhotoEntity entity)
        {
            if (entity == null) return null;

            return new PhotoListModel
            {
                Id = entity.Id,
                Photo = entity.Photo,
                ForeignGuid = entity.ForeignGuid
            };
        }

        public static PhotoDetailModel MapPhotoEntityToDetailModel(PhotoEntity entity)
        {
            if (entity == null) return null;

            return new PhotoDetailModel
                {
                    Id = entity.Id,
                    Photo = entity.Photo,
                    ForeignGuid = entity.ForeignGuid
            };
            }

        public static PhotoEntity MapPhotoDetailModelToEntity(PhotoDetailModel model)
        {
            return new PhotoEntity
            {
                Id = model.Id,
                Photo = model.Photo,
                ForeignGuid = model.ForeignGuid
            };
        }
    }
}
