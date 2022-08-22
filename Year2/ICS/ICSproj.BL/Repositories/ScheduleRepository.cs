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
    public class ScheduleRepository : IRepository<ScheduleDetailModel,ScheduleListModel>
    {
        private readonly IDbContextFactory<FestivalDbContext> _dbContextFactory;

        public ScheduleRepository(IDbContextFactory<FestivalDbContext> dbContextFactory)
        {
            _dbContextFactory = dbContextFactory;
        }


        public ICollection<ScheduleListModel> GetAll()
        {
            using var dbContext = _dbContextFactory.CreateDbContext();
            
            return dbContext.Schedule.Include(x => x.Band)
                .Include(x => x.Stage)
                .Select(e => ScheduleMapper.MapScheduleEntityToListModel(e)).ToArray();
        }

        public ScheduleDetailModel GetById(Guid id)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            ScheduleEntity entity = dbContext.Schedule.Include(x =>x.Band)
                .Include(x => x.Stage)
                .SingleOrDefault(t => t.Id == id);

            return ScheduleMapper.MapScheduleEntityToDetailModel(entity);
        }

        private IEnumerable<ScheduleDetailModel> GetByStageName(string stageName)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            IEnumerable<ScheduleDetailModel> retVal;
            try
            {
                retVal = dbContext.Schedule.Include(x => x.Band)
                    .Include(x => x.Stage)
                    .Select(e => ScheduleMapper.MapScheduleEntityToDetailModel(e)).ToArray()
                    .Where(t => t.StageName == stageName);
            }
            catch (Exception e)
            {
                return null;
            }

            return retVal;
        }

        public ScheduleEntity GetByModel(ScheduleDetailModel model)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            ScheduleEntity retVal;
            try
            {
                retVal = dbContext.Schedule.Include(x => x.Band)
                    .Include(x => x.Stage)
                    .Single(t => t.Band.Name == model.BandName && t.Stage.Name == model.StageName && t.PerformanceDateTime == model.PerformanceDateTime);
            }
            catch (Exception e)
            {
                return null;
            }

            return retVal;
        }

        private bool CanBePerformed(ScheduleDetailModel model)
        {
            var scheduleDetailModels = GetByStageName(model.StageName);

            foreach (var t in scheduleDetailModels.Where(t => 
                    (t.PerformanceDateTime <= model.PerformanceDateTime &&
                                                    t.PerformanceDateTime + t.PerformanceDuration >= model.PerformanceDateTime)  
                    // model starts in another performance
                    ||
                    // model ends in another performance
                   (t.PerformanceDateTime <= model.PerformanceDateTime + model.PerformanceDuration &&
                    t.PerformanceDateTime + t.PerformanceDuration >= model.PerformanceDateTime + model.PerformanceDuration))) 
            {
                if (t.BandName != model.BandName) return false;

                // is collision but should update
                using var dbContext = _dbContextFactory.CreateDbContext();
                DeleteByModel(model);
                return true;
            }

            return true;
        }

        public ScheduleDetailModel InsertOrUpdate(ScheduleDetailModel model)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            if (!CanBePerformed(model))
            {
                return null;
            }

            var entity = dbContext.Schedule
                .SingleOrDefault(x => x.Band.Name == model.BandName && x.Stage.Name == model.StageName && x.PerformanceDateTime == model.PerformanceDateTime);

            var band = dbContext.Bands.SingleOrDefault(t => t.Name == model.BandName);
            var stage = dbContext.Stages.SingleOrDefault(t => t.Name == model.StageName);

            if (band == null || stage == null)
                return null;

            entity ??= ScheduleMapper.MapScheduleDetailModelToEntity(model,
                dbContext.Bands.Single(t => t.Name == model.BandName),
                dbContext.Stages.Single(t => t.Name == model.StageName));

            band.PerformanceMapping.Add(entity);
            stage.PerformanceMapping.Add(entity);

            dbContext.Schedule.Update(entity);
            dbContext.Bands.Update(band);
            dbContext.Stages.Update(stage);
            dbContext.SaveChanges();

            return ScheduleMapper.MapScheduleEntityToDetailModel(entity);
        }

        public bool Delete(Guid id)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            var entity = new ScheduleEntity(id);

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

        public bool DeleteByModel(ScheduleDetailModel model)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();

            var entity = GetByModel(model);
            if (entity == null)
                return false;

            Delete(entity.Id);
            return true;

        }
    }
}
