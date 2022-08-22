using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ICSproj.DAL;
using ICSproj.DAL.Factories;
using Microsoft.EntityFrameworkCore;

namespace ICSproj.App.Factories
{
    public class SqlServerDbContextFactory : Microsoft.EntityFrameworkCore.IDbContextFactory<FestivalDbContext>
    {
        private readonly string _connectionString;

        public SqlServerDbContextFactory(string connectionString)
        {
            _connectionString = connectionString;
        }

        public FestivalDbContext CreateDbContext()
        {
            var optionsBuilder = new DbContextOptionsBuilder<FestivalDbContext>();
            optionsBuilder.UseSqlServer(_connectionString);
            return new FestivalDbContext(optionsBuilder.Options);
        }
    }
}
