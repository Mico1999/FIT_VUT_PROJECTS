using System;
using System.Linq;
using ICSproj.BL.Models;
using ICSproj.BL.Repositories;
using ICSproj.DAL;
using ICSproj.DAL.Factories;
using Microsoft.EntityFrameworkCore;
using Xunit;

namespace ICSproj.BL.Tests
{
    public class BandRepositoryTest : IDisposable
    {
        private readonly ScheduleRepository _scheduleRepositorySUT;
        private readonly BandRepository _bandRepositorySUT;
        private readonly StageRepository _stageRepositorySUT;
        private readonly IDbContextFactory<FestivalDbContext> _dbContextFactory;

        public BandRepositoryTest()
        {
            _dbContextFactory = new DbContextInMemoryFactory(nameof(BandRepositoryTest));
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
                Name = "Jaromir Nohavica"
            };
            var returnedModel = _bandRepositorySUT.InsertOrUpdate(band);

            var stage = new StageDetailModel()
            {
                Name = "Kvasov"
            };
            _stageRepositorySUT.InsertOrUpdate(stage);

            var model = new ScheduleDetailModel()
            {
                BandName = band.Name,
                StageName = stage.Name,
                PerformanceDateTime = DateTime.Today,
                PerformanceDuration = TimeSpan.FromHours(2)
            };
            _scheduleRepositorySUT.InsertOrUpdate(model);

            var model2 = new ScheduleDetailModel()
            {
                BandName = band.Name,
                StageName = stage.Name,
                PerformanceDateTime = DateTime.Today,
                PerformanceDuration = TimeSpan.FromHours(4)
            };
            _scheduleRepositorySUT.InsertOrUpdate(model2);

            Assert.NotNull(returnedModel);
        }


        [Fact]
        public void GetById_Seed()
        {
            Seed();
            using var dbContext = _dbContextFactory.CreateDbContext();
            var band1 = dbContext.Bands.Single(x => x.Name == "Foo");
            var band2 = dbContext.Bands.Single(x => x.Name == "Lorem Ipsum");

            var returnedModel = _bandRepositorySUT.GetById(band1.Id);
            var returnedModel2 = _bandRepositorySUT.GetById(band2.Id);

            Assert.NotNull(returnedModel);
            Assert.NotNull(returnedModel2);
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
            var returnedModel = _bandRepositorySUT.InsertOrUpdate(bandModel);

            _bandRepositorySUT.Delete(returnedModel.Id);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.False(dbxAssert.Bands.Any(i => i.Id == returnedModel.Id));

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
            var returnedModel = _bandRepositorySUT.InsertOrUpdate(bandModel);

            var stageModel = new StageDetailModel()
            {
                Name = "Random",
                Description = "Description"
            };
            _stageRepositorySUT.InsertOrUpdate(stageModel);

            var scheduleModel = new ScheduleDetailModel()
            {
                BandName = bandModel.Name,
                StageName = stageModel.Name,
                PerformanceDateTime = DateTime.Today,
                PerformanceDuration = TimeSpan.FromMinutes(80),
            };
            var retSched = _scheduleRepositorySUT.InsertOrUpdate(scheduleModel);

            var result = _bandRepositorySUT.Delete(retSched.StageId);
            Assert.False(result);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.True(dbxAssert.Bands.Any(i => i.Id == returnedModel.Id));
        }

        [Fact]
        public void InsertOrUpdate_ShouldUpdate()
        {
            Seed();

            var bandModel = new BandDetailModel()
            {
                Name = "Foo",
                Description = "Description"
            };

            var returnedModel = _bandRepositorySUT.InsertOrUpdate(bandModel);
            Assert.NotNull(returnedModel);
            Assert.Equal(3, _bandRepositorySUT.GetAll().Count());
        }

        [Fact]
        public void InsertOrUpdate_ShouldAdd()
        {
            Seed();
            Assert.Equal(3, _bandRepositorySUT.GetAll().Count());

            var bandModel = new BandDetailModel()
            {
                Name = "Nonexistent",
                Description = "Description",
            };

            var returnedModel = _bandRepositorySUT.InsertOrUpdate(bandModel);
            Assert.NotNull(returnedModel);
            Assert.Equal(4, _bandRepositorySUT.GetAll().Count());

        }

        [Fact]
        public void GetAll()
        {
            Seed();
            var returned = _bandRepositorySUT.GetAll();
            Assert.Equal(3, returned.Count());
        }

        [Fact]
        public void GetAll_Empty()
        {
            var returned = _bandRepositorySUT.GetAll();
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
