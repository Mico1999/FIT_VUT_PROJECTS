using System;
using System.Linq;
using ICSproj.DAL.Entities;
using ICSproj.DAL;
using ICSproj.DAL.Factories;
using Microsoft.EntityFrameworkCore;
using Xunit;

namespace ICSproj.Tests
{
    public class FestivalDbContextTest : IDisposable
    {
        private readonly FestivalDbContext _dbContextSUT;
        private readonly DbContextInMemoryFactory _dbContextFactory;

        public FestivalDbContextTest()
        {
            _dbContextFactory = new DbContextInMemoryFactory(nameof(FestivalDbContextTest));
            _dbContextSUT = _dbContextFactory.CreateDbContext();
        }

        [Fact]
        public void AddNew_Band_Persisted()
        {
            //Arrange
            var bandEntity = new BandEntity()
            {
                Description = "This is my favorite band",
                DescriptionLong = "Lorem ipsum",
                Genre = "Rock",
                Name = "21 pilots",
                OriginCountry = "USA",
                PerformanceMapping =
                {
                    new ScheduleEntity()
                    {
                        PerformanceDateTime =  new DateTime(2021, 6, 2, 19,00,00),
                        PerformanceDuration = TimeSpan.FromHours(2),
                        Stage = new StageEntity(){Name = "East", Description = "Main"}
                    },
                    new ScheduleEntity()
                    {
                        PerformanceDateTime =  new DateTime(2021, 6, 8, 21,00,00),
                        PerformanceDuration = TimeSpan.FromHours(2),
                        Stage = new StageEntity(){Name = "West", Description = "Main"}
                    }
                }
            };

            //Act
            _dbContextSUT.Bands.Add(bandEntity);
            _dbContextSUT.SaveChanges();

            //Assert
            using var dbx = _dbContextFactory.CreateDbContext();
            var bandFromDatabase = dbx.Bands
                .Include(band => band.PerformanceMapping)
                .ThenInclude(ScheduleEntity => ScheduleEntity.Stage)
                .FirstOrDefault(i => i.Id == bandEntity.Id);
            Assert.True(bandFromDatabase.Equals(bandEntity));
        }

        [Fact]
        public void AddNew_Stage_Persisted()
        {
            //Arrange
            var stageEntity = new StageEntity()
            {
                Description = "Western", 
                Name = "Main Stage",
                PerformanceMapping =
                {
                    new ScheduleEntity()
                    {
                        PerformanceDateTime =  new DateTime(2021, 7, 3, 20,30,00),
                        PerformanceDuration = TimeSpan.FromHours(2),
                        Band = new BandEntity()
                        {
                            Name = "Separ", 
                            Description = "Michal Kmet", 
                            Genre = "Rap", 
                            OriginCountry = "Slovakia", 
                            DescriptionLong = "Je to lepsie jak hocico si si dal dnes"
                        }
                    },
                    new ScheduleEntity()
                    {
                    PerformanceDateTime =  new DateTime(2021, 7, 3, 22,30,00),
                    PerformanceDuration = TimeSpan.FromHours(2),
                    Band = new BandEntity()
                    {
                        Name = "Coldplay", 
                        Description = "British rock band formed in London in 1996", 
                        Genre = "Rock", 
                        OriginCountry = "England", 
                        DescriptionLong = "www.coldplay.com"
                    }
                }
                }
            };

            //Act
            _dbContextSUT.Stages.Add(stageEntity);
            _dbContextSUT.SaveChanges();

            //Assert
            using var dbx = _dbContextFactory.CreateDbContext();
            var stageFromDatabase = dbx.Stages
                .Include(stage => stage.PerformanceMapping)
                .ThenInclude(ScheduleEntity => ScheduleEntity.Band)
                .FirstOrDefault(i => i.Id == stageEntity.Id);
            Assert.True(stageFromDatabase.Equals(stageEntity));
        }

        [Fact]
        public void AddNew_Schedule_Persisted()
        {
            var stageEntity = new StageEntity() { Description = "Western", Name = "Main Stage" };
            var bandEntity = new BandEntity()
            {
                Description = "This is my favorite band",
                DescriptionLong = "Lorem ipsum",
                Genre = "Rock",
                Name = "21 pilots",
                OriginCountry = "USA"
            };

            var scheduleEntity = new ScheduleEntity()
            {
                Band = bandEntity, 
                Stage = stageEntity, 
                PerformanceDateTime = DateTime.Today,
                PerformanceDuration = TimeSpan.FromHours(2)
                
            };

            _dbContextSUT.Schedule.Add(scheduleEntity);
            _dbContextSUT.SaveChanges();


            using var dbx = _dbContextFactory.CreateDbContext();
            var scheduleFromDatabase = dbx.Schedule
                .Include(schedule => schedule.Band)
                .Include(schedule => schedule.Stage)
                .FirstOrDefault(i => i.Id == scheduleEntity.Id);
            Assert.True(scheduleFromDatabase.Equals(scheduleEntity));
        }

        public void Dispose()
        {
            _dbContextSUT.Dispose();
        }
    }
}
