using System;
using System.Collections.Generic;
using System.Text;
using ICSproj.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace ICSproj.DAL
{
    public class FestivalDbContext : DbContext
    {
        public FestivalDbContext(DbContextOptions options) : base(options)
        {
        }

        public DbSet<BandEntity> Bands { get; set; }
        public DbSet<StageEntity> Stages { get; set; }
        public DbSet<ScheduleEntity> Schedule { get; set; }
        public DbSet<PhotoEntity> Photos { get; set; }
    }
}
