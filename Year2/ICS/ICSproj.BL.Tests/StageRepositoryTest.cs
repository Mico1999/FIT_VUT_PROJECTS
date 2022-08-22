using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ICSproj.BL.Repositories;
using ICSproj.DAL.Entities;
using ICSproj.BL.Mappers;
using ICSproj.BL.Models;
using ICSproj.DAL.Factories;
using Xunit;


namespace ICSproj.BL.Tests
{
    public class StageRepositoryTest : IDisposable
    {
        private readonly ScheduleRepository _scheduleRepositorySUT;
        private readonly BandRepository _bandRepositorySUT;
        private readonly StageRepository _stageRepositorySUT;
        private readonly DbContextInMemoryFactory _dbContextFactory;

        public StageRepositoryTest()
        {
            _dbContextFactory = new DbContextInMemoryFactory(nameof(StageRepositoryTest));
            using var dbx = _dbContextFactory.CreateDbContext();
            dbx.Database.EnsureCreated();

            _scheduleRepositorySUT = new ScheduleRepository(_dbContextFactory);
            _bandRepositorySUT = new BandRepository(_dbContextFactory);
            _stageRepositorySUT = new StageRepository(_dbContextFactory);
        }

        [Fact]
        public void InsertOrUpdate_Create()
        {
            var stage = new StageDetailModel()
            {
                Name = "First"
            };
            var returnedModel = _stageRepositorySUT.InsertOrUpdate(stage);

            Assert.NotNull(returnedModel);
        }

        [Fact]
        public void GetById_Seed()
        {
            Seed();
            using var dbContext = _dbContextFactory.CreateDbContext();
            var stage1 = dbContext.Stages.Single(x => x.Name == "Bar");
            var stage2 = dbContext.Stages.Single(x => x.Name == "Stage Name");

            var returnedModel = _stageRepositorySUT.GetById(stage1.Id);
            var returnedModel2 = _stageRepositorySUT.GetById(stage2.Id);

            Assert.NotNull(returnedModel);
            Assert.NotNull(returnedModel2);
        }

        [Fact]
        public void Get_By_Name()
        {
            var stageModel = new StageDetailModel()
            {
                Name = "This stage",
                Description = "Description"
            };
            var returnedModel = _stageRepositorySUT.InsertOrUpdate(stageModel);
            var returnedStage = _stageRepositorySUT.GetByName(stageModel.Name);
            Assert.Equal(returnedStage.Id, returnedModel.Id);
            Assert.Equal(returnedStage.Photos.Count(), returnedModel.Photos.Count());
            Assert.Equal(returnedStage.Schedule.Count(), returnedModel.Schedule.Count());
        }

        [Fact]
        public void Delete_ShouldDelete()
        {
            var stageModel = new StageDetailModel()
            {
                Name = "This stage",
                Description = "Description"
            };
            var returnedModel = _stageRepositorySUT.InsertOrUpdate(stageModel);

            _stageRepositorySUT.Delete(returnedModel.Id);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.False(dbxAssert.Stages.Any(i => i.Id == returnedModel.Id));
        }

        [Fact]
        public void Delete_ShouldNotDelete()
        {
            var bandModel = new BandDetailModel()
            {
                Name = "The Wombats",
                Description = "Description",
                DescriptionLong = "Longer description",
                Genre = "Metal",
                OriginCountry = "DE"
            };
            _bandRepositorySUT.InsertOrUpdate(bandModel);

            var stageModel = new StageDetailModel()
            {
                Name = "Random",
                Description = "Description"
            };
            var returnedModel = _stageRepositorySUT.InsertOrUpdate(stageModel);

            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = bandModel.Name,
                StageName = stageModel.Name,
                PerformanceDateTime = DateTime.Today,
                PerformanceDuration = TimeSpan.FromMinutes(80),
            };
            var retunedSchedule = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);

            var result = _stageRepositorySUT.Delete(retunedSchedule.BandId); //WrongID
            Assert.False(result);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.True(dbxAssert.Stages.Any(i => i.Id == returnedModel.Id));
        }

        [Fact]
        public void InsertOrUpdate_ShouldUpdate()
        {
            Seed();

            var stageModel = new StageDetailModel()
            {
                Name = "Stage Name",
                Description = "Description"
            };

            var returnedModel = _stageRepositorySUT.InsertOrUpdate(stageModel);
            Assert.NotNull(returnedModel);
            Assert.Equal(3, _stageRepositorySUT.GetAll().Count());
        }

        [Fact]
        public void InsertOrUpdate_ShouldAdd()
        {
            Seed();
            Assert.Equal(3, _scheduleRepositorySUT.GetAll().Count());
            Assert.Equal(3, _stageRepositorySUT.GetAll().Count());
            Assert.Equal(3, _bandRepositorySUT.GetAll().Count());

            var stageModel = new StageDetailModel()
            {
                Name = "Nonexistent",
                Description = "Description",
            };

            var returnedModel = _stageRepositorySUT.InsertOrUpdate(stageModel);
            Assert.NotNull(returnedModel);
            Assert.Equal(4, _stageRepositorySUT.GetAll().Count());

        }

        [Fact]
        public void GetAll()
        {
            Seed();
            var returned = _stageRepositorySUT.GetAll();
            Assert.Equal(3, returned.Count());
        }

        [Fact]
        public void GetAll_Empty()
        {
            var returned = _stageRepositorySUT.GetAll();
            Assert.Empty(returned);
        }

        private void Seed()
        {
            var bandModel = new BandDetailModel()
            {
                Name = "Lorem Ipsum",
                Description = "Description",
                DescriptionLong = "Longer description",
                Genre = "Jazz",
                OriginCountry = "UK"
            };
            _bandRepositorySUT.InsertOrUpdate(bandModel);
            var bandModel2 = new BandDetailModel()
            {
                Name = "Foo",
                Description = "Description",
                DescriptionLong = "Longer description",
                Genre = "Rock",
                OriginCountry = "US"
            };
            _bandRepositorySUT.InsertOrUpdate(bandModel2);
            var bandModel3 = new BandDetailModel()
            {
                Name = "Svieca vo vetre",
                Description = "Description",
                DescriptionLong = "Longer description",
                Genre = "Punk-Rock",
                OriginCountry = "SK"
            };
            _bandRepositorySUT.InsertOrUpdate(bandModel3);
            // =============================================
            var stageModel = new StageDetailModel()
            {
                Name = "Stage Name",
                Description = "Description"
            };
            _stageRepositorySUT.InsertOrUpdate(stageModel);

            var stageModel2 = new StageDetailModel()
            {
                Name = "Bar",
                Description = "Description"
            };
            _stageRepositorySUT.InsertOrUpdate(stageModel2);
            var stageModel3 = new StageDetailModel()
            {
                Name = "Pekny stage",
                Description = "Description"
            };
            _stageRepositorySUT.InsertOrUpdate(stageModel3);
            // =============================================
            var scheduleModel1 = new ScheduleDetailModel()
            {
                BandName = bandModel.Name,
                StageName = stageModel.Name,
                PerformanceDuration = TimeSpan.FromMinutes(90),
                PerformanceDateTime = DateTime.Today
            };
            _scheduleRepositorySUT.InsertOrUpdate(scheduleModel1);

            var scheduleModel2 = new ScheduleDetailModel()
            {
                BandName = bandModel2.Name,
                StageName = stageModel2.Name,
                PerformanceDuration = TimeSpan.FromMinutes(80),
                PerformanceDateTime = DateTime.Today
            };
            _scheduleRepositorySUT.InsertOrUpdate(scheduleModel2);

            var scheduleModel3 = new ScheduleDetailModel()
            {
                BandName = bandModel3.Name,
                StageName = stageModel3.Name,
                PerformanceDuration = TimeSpan.FromMinutes(120),
                PerformanceDateTime = DateTime.Today
            };
            var returnedModel3 = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel3);

        }
        public void Dispose()
        {
            using var dbx = _dbContextFactory.CreateDbContext();
            dbx.Database.EnsureDeleted();
        }
    }
}
