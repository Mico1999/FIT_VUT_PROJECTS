using System;
using System.Collections.Generic;
using System.Linq;
using ICSproj.BL.Mappers;
using ICSproj.BL.Models;
using ICSproj.DAL;
using ICSproj.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace ICSproj.BL.Repositories
{
    public class BandRepository : IRepository<BandDetailModel, BandListModel>
    {
        private readonly IDbContextFactory<FestivalDbContext> _dbContextFactory;

        public BandRepository(IDbContextFactory<FestivalDbContext> dbContextFactory)
        {
            _dbContextFactory = dbContextFactory;
        }

        public BandDetailModel InsertOrUpdate(BandDetailModel model)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            var entity = dbContext.Bands.SingleOrDefault(x => x.Name == model.Name);

            if (entity == null)
            {
                entity = BandMapper.MapBandDetailModelToEntity(model);
            }

            dbContext.Bands.Update(entity);
            dbContext.SaveChanges();

            return BandMapper.MapBandEntityToDetailModel(entity);
        }

        public BandDetailModel GetById(Guid id)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            BandEntity entity = dbContext.Bands.Include(x => x.Photos)
                .Include(x => x.PerformanceMapping)
                .ThenInclude(x => x.Stage)
                .SingleOrDefault(t => t.Id == id);

            return BandMapper.MapBandEntityToDetailModel(entity);
        }

        public BandDetailModel GetByName(string bandName)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            BandEntity entity = dbContext.Bands.Single(t => t.Name == bandName);

            return BandMapper.MapBandEntityToDetailModel(entity);
        }

        public bool Delete(Guid id)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            var entity = new BandEntity(id);

            dbContext.Remove(entity);
            try
            {
                dbContext.SaveChanges();
            }
            catch (Exception)
            {
                return false;
            }

            return true;
        }

        public ICollection<BandListModel> GetAll()
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            return dbContext.Bands
                .Select(e => BandMapper.MapBandEntityToListModel(e)).ToArray();
        }
    }
}
