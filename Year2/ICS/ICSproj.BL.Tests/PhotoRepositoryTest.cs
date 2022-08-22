using System;
using System.Collections.Generic;
using System.Linq;
using ICSproj.BL.Mappers;
using ICSproj.BL.Models;
using Xunit;
using ICSproj.BL.Repositories;
using ICSproj.DAL.Entities;
using System.Drawing;
using System.IO;
using System.Net.Mime;
using System.Reflection;
using ICSproj.DAL;
using ICSproj.DAL.Factories;
using Microsoft.EntityFrameworkCore;

namespace ICSproj.BL.Tests
{
    public class PhotoRepositoryTest : IDisposable
    {
        private readonly StageRepository _stageRepositorySUT;
        private readonly PhotoRepository _photoRepositorySUT;
        private readonly DbContextInMemoryFactory _dbContextFactory;

        public PhotoRepositoryTest()
        {
            _dbContextFactory = new DbContextInMemoryFactory(nameof(PhotoRepositoryTest));
            using var dbx = _dbContextFactory.CreateDbContext();
            dbx.Database.EnsureCreated();

            _stageRepositorySUT = new StageRepository(_dbContextFactory);
            _photoRepositorySUT = new PhotoRepository(_dbContextFactory);
        }

        [Fact]
        public void GetAll_NothingInDatabase()
        {
            Assert.Empty(_photoRepositorySUT.GetAll());
        }

        [Fact]
        public void InsertOrUpdate_ShouldInsert()
        {

            Image img = Image.FromFile(@"../../../Photos/33583_uu-1581261011.jpg");
            byte[] arr;
            using (MemoryStream ms = new MemoryStream())
            {
                img.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                arr = ms.ToArray();
            }

            var model = new PhotoDetailModel()
            {
                Photo = arr
            };
            var returnedModel = _photoRepositorySUT.InsertOrUpdate(model);
            Assert.NotNull(returnedModel);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.True(dbxAssert.Photos.Any(i => i.Id == returnedModel.Id));

        }

        [Fact]
        public void InsertOrUpdate_ShouldNotInsert()
        {

            Image img = Image.FromFile(@"../../../Photos/33583_uu-1581261011.jpg");
            byte[] arr;
            using (MemoryStream ms = new MemoryStream())
            {
                img.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                arr = ms.ToArray();
            }

            var model = new PhotoDetailModel()
            {
                Photo = arr
            };
            var returnedModel = _photoRepositorySUT.InsertOrUpdate(model);
            Assert.NotNull(returnedModel);

            var theSameModel = new PhotoDetailModel()
            {
                Photo = arr
            };

            returnedModel = _photoRepositorySUT.InsertOrUpdate(theSameModel);
            Assert.Single(_photoRepositorySUT.GetAll());

        }

        [Fact]
        public void InsertOrUpdate_ShouldInsert2()
        {
            Seed();
            Assert.Equal(2, _photoRepositorySUT.GetAll().Count());

        }

        [Fact]
        public void InsertOrUpdate_ShouldNotInsert2()
        {
            Seed();
            Assert.Equal(2, _photoRepositorySUT.GetAll().Count());

            Image img = Image.FromFile(@"../../../Photos/33583_uu-1581261011.jpg");
            byte[] arr;
            using (MemoryStream ms = new MemoryStream())
            {
                img.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                arr = ms.ToArray();
            }

            var model = new PhotoDetailModel()
            {
                Photo = arr
            };
            var returnedModel = _photoRepositorySUT.InsertOrUpdate(model);
            Assert.Equal(2, _photoRepositorySUT.GetAll().Count());

        }

        [Fact]
        public void Delete_ShouldNotDelete()
        {
            bool ret = _photoRepositorySUT.Delete(Guid.Empty);
            Assert.False(ret);
        }

        [Fact]
        public void Delete_ShouldDelete()
        {
            Seed();
            var list = _photoRepositorySUT.GetAll().ToList();
            bool ret = _photoRepositorySUT.Delete(list[0].Id);
            Assert.True(ret);
            Assert.Single(_photoRepositorySUT.GetAll());
        }

        [Fact]
        public void GetById()
        {
            Image img = Image.FromFile(@"../../../Photos/photo-1525609004556-c46c7d6cf023.jpg");
            byte[] arr;
            using (MemoryStream ms = new MemoryStream())
            {
                img.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                arr = ms.ToArray();
            }

            var model = new PhotoDetailModel()
            {
                Photo = arr
            };
            var returnedModel = _photoRepositorySUT.InsertOrUpdate(model);

            var theSame = _photoRepositorySUT.GetById(returnedModel.Id);

            Assert.Equal(returnedModel.Photo,theSame.Photo);
        }

        [Fact]
        public void CheckPhotosInOtherEntities()
        {
            Image img = Image.FromFile(@"../../../Photos/photo-1525609004556-c46c7d6cf023.jpg");
            byte[] arr;
            using (MemoryStream ms = new MemoryStream())
            {
                img.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                arr = ms.ToArray();
            }

            var model = new PhotoDetailModel()
            {
                Photo = arr
            };
            var returnedModel = _photoRepositorySUT.InsertOrUpdate(model);


            Image img2 = Image.FromFile(@"../../../Photos/close-up-of-tulips-blooming-in-field-royalty-free-image-1584131616.jpg");
            byte[] arr2;
            using (MemoryStream ms = new MemoryStream())
            {
                img2.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                arr2 = ms.ToArray();
            }

            var model2 = new PhotoDetailModel()
            {
                Photo = arr2
            };
            var returnedModel2 = _photoRepositorySUT.InsertOrUpdate(model2);

            var list = new List<PhotoDetailModel>();
            list.Add(returnedModel);

            var stageModel = new StageDetailModel()
            {
                Name = "Stage Name",
                Description = "Description",
                Photos = list
            };
            _stageRepositorySUT.InsertOrUpdate(stageModel);

            using var dbxAssert = _dbContextFactory.CreateDbContext();
            Assert.Single(dbxAssert.Stages.Include(x => x.Photos)
                .Single(x => x.Name == stageModel.Name).Photos);
        }

        private void Seed()
        {
            Image img = Image.FromFile(@"../../../Photos/33583_uu-1581261011.jpg");
            byte[] arr;
            using (MemoryStream ms = new MemoryStream())
            {
                img.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                arr = ms.ToArray();
            }

            var model = new PhotoDetailModel()
            {
                Photo = arr
            };
            var returnedModel = _photoRepositorySUT.InsertOrUpdate(model);


            Image img2 = Image.FromFile(@"../../../Photos/close-up-of-tulips-blooming-in-field-royalty-free-image-1584131616.jpg");
            byte[] arr2;
            using (MemoryStream ms = new MemoryStream())
            {
                img2.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                arr2 = ms.ToArray();
            }

            var model2 = new PhotoDetailModel()
            {
                Photo = arr2
            };
            var returnedModel2 = _photoRepositorySUT.InsertOrUpdate(model2);
        }
        public void Dispose()
        {
            using var dbx = _dbContextFactory.CreateDbContext();
            dbx.Database.EnsureDeleted();
        }
    }
}
