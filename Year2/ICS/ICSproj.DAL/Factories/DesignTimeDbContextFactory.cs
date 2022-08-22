using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;

namespace ICSproj.DAL.Factories
{
    public class DesignTimeDbContextFactory : IDesignTimeDbContextFactory<FestivalDbContext>
    {
        public FestivalDbContext CreateDbContext(string[] args)
        {
            var builder = new DbContextOptionsBuilder<FestivalDbContext>();
            builder.UseSqlServer(
                @"Data Source=(LocalDB)\MSSQLLocalDB;
                Initial Catalog = TasksDB;
                MultipleActiveResultSets = True;
                Integrated Security = True; ");
            return new FestivalDbContext(builder.Options);
        }
    }
}
