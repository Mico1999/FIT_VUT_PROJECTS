using System;
using System.Collections.Generic;
using System.Linq;
using ICSproj.BL.Mappers;
using ICSproj.BL.Models;
using ICSproj.DAL.Entities;
using ICSproj.DAL;
using Microsoft.EntityFrameworkCore;

namespace ICSproj.BL.Repositories
{
    public class PhotoRepository : IRepository<PhotoDetailModel, PhotoListModel>
    {
        private readonly IDbContextFactory<FestivalDbContext> _dbContextFactory;

        public PhotoRepository(IDbContextFactory<FestivalDbContext> dbContextFactory)
        {
            _dbContextFactory = dbContextFactory;
        }
        public ICollection<PhotoListModel> GetAll()
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            return dbContext.Photos.Select(x => PhotoMapper.MapPhotoEntityToListModel(x)).ToArray();
        }

        public PhotoDetailModel GetById(Guid id)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            PhotoEntity entity = dbContext.Photos.SingleOrDefault(t => t.Id == id);

            return PhotoMapper.MapPhotoEntityToDetailModel(entity);
        }

        public PhotoDetailModel GetByForeignId(Guid id)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            PhotoEntity entity = dbContext.Photos.SingleOrDefault(t => t.ForeignGuid == id);

            return PhotoMapper.MapPhotoEntityToDetailModel(entity);
        }

        public PhotoDetailModel InsertOrUpdate(PhotoDetailModel model)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            var entity = dbContext.Photos.SingleOrDefault(x => x.ForeignGuid == model.ForeignGuid && x.Photo == model.Photo);

            if (entity == null)
            {
                entity = PhotoMapper.MapPhotoDetailModelToEntity(model);
            }

            dbContext.Photos.Update(entity);
            dbContext.SaveChanges();

            return PhotoMapper.MapPhotoEntityToDetailModel(entity);
        }

        public bool Delete(Guid id)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            var entity = new PhotoEntity(id);

            dbContext.Remove(entity);
            try
            {
                dbContext.SaveChanges();
            }
            catch (Exception e)
            {
                return false;
            }
            return true;
        }

        public bool DeleteForeign(Guid id)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            PhotoEntity entity = dbContext.Photos.SingleOrDefault(t => t.ForeignGuid == id);

            if (entity == null)
            {
                return false;
            }

            dbContext.Remove(entity);
            try
            {
                dbContext.SaveChanges();
            }
            catch (Exception e)
            {
                return false;
            }
            return true;
        }
    }
}
