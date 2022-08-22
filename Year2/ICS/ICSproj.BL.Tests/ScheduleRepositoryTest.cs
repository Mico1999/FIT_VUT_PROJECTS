using System;
using System.Collections.Generic;
using System.Linq;
using ICSproj.BL.Mappers;
using ICSproj.BL.Models;
using Xunit;
using ICSproj.BL.Repositories;
using ICSproj.DAL.Entities;
using ICSproj.DAL.Factories;

namespace ICSproj.BL.Tests
{
    public sealed class ScheduleRepositoryTest : IDisposable
    {
        private readonly ScheduleRepository _scheduleRepositorySUT;
        private readonly BandRepository _bandRepositorySUT;
        private readonly StageRepository _stageRepositorySUT;
        private readonly DbContextInMemoryFactory _dbContextFactory;

        public ScheduleRepositoryTest()
        {
            _dbContextFactory = new DbContextInMemoryFactory(nameof(ScheduleRepositoryTest));
            using var dbx = _dbContextFactory.CreateDbContext();
            dbx.Database.EnsureCreated();

            _scheduleRepositorySUT = new ScheduleRepository(_dbContextFactory);
            _bandRepositorySUT = new BandRepository(_dbContextFactory);
            _stageRepositorySUT = new StageRepository(_dbContextFactory);
        }

        [Fact]
        public void InsertOrUpdate_Create()
        {
            var band = new BandDetailModel()
            {
                Name = "21 pilots"
            };
            _bandRepositorySUT.InsertOrUpdate(band);

            var stage = new StageDetailModel()
            {

                Name = "Cool stage"
            };
            _stageRepositorySUT.InsertOrUpdate(stage);

            var model = new ScheduleDetailModel()
            {
                BandName = band.Name,
                StageName = stage.Name,
                PerformanceDateTime = DateTime.Today,
                PerformanceDuration = TimeSpan.FromHours(2)
            };

            var returnedModel = _scheduleRepositorySUT.InsertOrUpdate(model);

            Assert.NotNull(returnedModel);
        }

        [Fact]
        public void GetById_Create()
        {
            Seed();
            using var dbContext = _dbContextFactory.CreateDbContext();
            var band1 = dbContext.Bands.Single(x => x.Name == "Foo");
            var band2 = dbContext.Bands.Single(x => x.Name == "Lorem Ipsum");
            var stage1 = dbContext.Stages.Single(x => x.Name == "Bar");
            var stage2 = dbContext.Stages.Single(x => x.Name == "Stage Name");
            var schedule1 = dbContext.Schedule.Single(x => x.BandId == band1.Id && x.StageId == stage1.Id);
            var schedule2 = dbContext.Schedule.Single(x => x.BandId == band2.Id && x.StageId == stage2.Id);

            Assert.NotNull(schedule1);
            var result = _scheduleRepositorySUT.GetById(schedule1.Id);
            Assert.Equal(result.BandName, band1.Name);
            Assert.Equal(result.StageName, stage1.Name);

            Assert.NotNull(schedule2);
            result = _scheduleRepositorySUT.GetById(schedule2.Id);
            Assert.Equal(result.BandName, band2.Name);
            Assert.Equal(result.StageName, stage2.Name);
        }

        [Fact]
        public void InsertOrUpdate_ShouldAdd()
        {
            Seed();

            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = "Svieca vo vetre",
                StageName = "Bar",
                PerformanceDuration = TimeSpan.FromMinutes(90),
                PerformanceDateTime = DateTime.Today + TimeSpan.FromDays(1)
            };
            var returnedModel = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);
            Assert.Equal(4, _scheduleRepositorySUT.GetAll().Count());
            Assert.Equal(3, _stageRepositorySUT.GetAll().Count());
            Assert.Equal(3, _bandRepositorySUT.GetAll().Count());
        }

        [Fact]
        public void InsertOrUpdate_ShouldNotAdd()
        {
            Seed();

            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = "Ahoj", // Not in seed
                StageName = "Pekny stage",
                PerformanceDuration = TimeSpan.FromMinutes(20),
                PerformanceDateTime = DateTime.Today
            };
            var returnedModel = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);
            Assert.Null(returnedModel);
            Assert.Equal(3, _scheduleRepositorySUT.GetAll().Count());
            Assert.Equal(3, _stageRepositorySUT.GetAll().Count());
            Assert.Equal(3, _bandRepositorySUT.GetAll().Count());
        }

        [Fact]
        public void InsertOrUpdate_ShouldUpdate()
        {
            Seed();

            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = "Svieca vo vetre",
                StageName = "Pekny stage",
                PerformanceDuration = TimeSpan.FromMinutes(20),
                PerformanceDateTime = DateTime.Today
            };
            var returnedModel = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);
            Assert.NotNull(returnedModel);
            Assert.Equal(3, _stageRepositorySUT.GetAll().Count());
            Assert.Equal(3, _bandRepositorySUT.GetAll().Count());
            Assert.Equal(3, _scheduleRepositorySUT.GetAll().Count());
        }

        [Fact]
        public void DeleteByModel_ShouldDelete()
        {
            Seed();
            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = "Lorem Ipsum",
                StageName = "Stage Name",
                PerformanceDateTime = DateTime.Today
            };

            _scheduleRepositorySUT.DeleteByModel(scheduleModel);
            Assert.Equal(2, _scheduleRepositorySUT.GetAll().Count());
        }

        [Fact]
        public void DeleteByModel_ShouldNotDelete()
        {
            Seed();
            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = "Lorem Ipsum",
                StageName = "Stage Name",
                PerformanceDateTime = DateTime.Today + TimeSpan.FromDays(1) // Tomorrow
            };

            var returnedModel = _scheduleRepositorySUT.DeleteByModel(scheduleModel);
            Assert.False(returnedModel);
            Assert.Equal(3, _scheduleRepositorySUT.GetAll().Count());
        }

        [Fact]
        public void Delete_ShouldDelete()
        {
            var bandModel = new BandDetailModel()
            {
                Name = "Random",
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
            _stageRepositorySUT.InsertOrUpdate(stageModel);

            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = "Random",
                StageName = "Random",
                PerformanceDateTime = DateTime.Today,
                PerformanceDuration = TimeSpan.FromMinutes(80),
            };
            var returnedModel = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);
            _scheduleRepositorySUT.Delete(returnedModel.Id);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.False(dbxAssert.Schedule.Any(i => i.Id == returnedModel.Id));

        }

        [Fact]
        public void Delete_ShouldNotDelete()
        {
            var bandModel = new BandDetailModel()
            {
                Name = "Random",
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
            _stageRepositorySUT.InsertOrUpdate(stageModel);

            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = "Random",
                StageName = "Random",
                PerformanceDateTime = DateTime.Today,
                PerformanceDuration = TimeSpan.FromMinutes(80),
            };
            var returnedModel = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);
            var result = _scheduleRepositorySUT.Delete(returnedModel.BandId); //WrongID
            Assert.False(result);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.True(dbxAssert.Schedule.Any(i => i.Id == returnedModel.Id));
        }

        [Fact]
        public void GetAll()
        {
            Seed();
            var returned = _scheduleRepositorySUT.GetAll();
            Assert.Equal(3, returned.Count());
        }

        [Fact]
        public void GetAll_Empty()
        {
            var returned = _scheduleRepositorySUT.GetAll();
            Assert.Empty(returned);
        }

        [Fact]
        public void CheckScheduleInOtherEntities_SeedAndCreate()
        {
            Seed();
            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = "Lorem Ipsum",
                StageName = "Bar",
                PerformanceDuration = TimeSpan.FromMinutes(90),
                PerformanceDateTime = DateTime.Today + TimeSpan.FromHours(3)
            };
            var returnedModel = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.Equal(4, dbxAssert.Schedule.Count());

            var band = _bandRepositorySUT.GetById(returnedModel.BandId);
            Assert.Equal(2, band.Schedule.Count);

            var stage = _stageRepositorySUT.GetById(returnedModel.StageId);
            Assert.Equal(2, stage.Schedule.Count);
        }

        [Fact]
        public void CheckScheduleInOtherEntities_SeedAndDelete()
        {
            Seed();
            var deleteModel = new ScheduleDetailModel()
            {
                BandName = "Foo",
                StageName = "Bar",
                PerformanceDateTime = DateTime.Today
            };
            var returnedModel = _scheduleRepositorySUT.GetByModel(deleteModel);

            _scheduleRepositorySUT.DeleteByModel(deleteModel);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.Equal(2, dbxAssert.Schedule.Count());

            var band = _bandRepositorySUT.GetById(returnedModel.BandId);
            Assert.Empty(band.Schedule);

            var stage = _stageRepositorySUT.GetById(returnedModel.StageId);
            Assert.Empty(stage.Schedule);

        }

        [Fact]
        public void CheckScheduleInOtherEntities_Seed()
        {
            Seed();

            var model = new ScheduleDetailModel()
            {
                BandName = "Lorem Ipsum",
                StageName = "Stage Name",
                PerformanceDateTime = DateTime.Today
            };
            var returnedModel = _scheduleRepositorySUT.GetByModel(model);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.Equal(3, dbxAssert.Schedule.Count());

            var band = _bandRepositorySUT.GetById(returnedModel.BandId);
            Assert.NotEmpty(band.Schedule);

            var stage = _stageRepositorySUT.GetById(returnedModel.StageId);
            Assert.NotEmpty(stage.Schedule);
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
            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = bandModel.Name,
                StageName = stageModel.Name,
                PerformanceDuration = TimeSpan.FromMinutes(90),
                PerformanceDateTime = DateTime.Today
            };
            var returnedModel = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);

            var scheduleModel2 = new ScheduleDetailModel()
            {
                BandName = bandModel2.Name,
                StageName = stageModel2.Name,
                PerformanceDuration = TimeSpan.FromMinutes(80),
                PerformanceDateTime = DateTime.Today
            };
            var returnedModel2 = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel2);

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