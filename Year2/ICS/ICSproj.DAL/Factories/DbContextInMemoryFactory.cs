using Microsoft.EntityFrameworkCore;

namespace ICSproj.DAL.Factories
{
    public class DbContextInMemoryFactory : IDbContextFactory<FestivalDbContext>
    {
        private string _databaseName;

        public DbContextInMemoryFactory(string databaseName)
        {
            _databaseName = databaseName;
        }

        public FestivalDbContext CreateDbContext()
        {
            DbContextOptionsBuilder<FestivalDbContext>? contextOptionsBuilder = new DbContextOptionsBuilder<FestivalDbContext>();
            contextOptionsBuilder.UseInMemoryDatabase(_databaseName);
            return new FestivalDbContext(contextOptionsBuilder.Options);
        }
    }
}