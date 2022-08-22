using System;
using System.Windows;
using ICSproj.App.Extensions;
using ICSproj.App.Factories;
using ICSproj.App.Services;
using ICSproj.App.ViewModels;
using ICSproj.App.ViewModels.Interfaces;
using ICSproj.BL.Models;
using ICSproj.BL.Repositories;
using ICSproj.DAL;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace ICSproj.App
{

    public partial class App : Application
    {
        private readonly IHost _host;

        public App()
        {
            _host = Host.CreateDefaultBuilder()
                .ConfigureAppConfiguration(ConfigureAppConfiguration)
                .ConfigureServices((context, services) => { ConfigureServices(context.Configuration, services); })
                .Build();
        }
        private static void ConfigureAppConfiguration(HostBuilderContext context, IConfigurationBuilder builder)
        {
            builder.AddJsonFile(@"appsettings.json", false, true);
        }
        private static void ConfigureServices(IConfiguration configuration,
            IServiceCollection services)
        {
            services.AddSingleton<MainWindow>();


            services.AddSingleton<IRepository<BandDetailModel, BandListModel>, BandRepository>();
            services.AddSingleton<IRepository<StageDetailModel, StageListModel>, StageRepository>();
            services.AddSingleton<IRepository<PhotoDetailModel, PhotoListModel>, PhotoRepository>();
            services.AddSingleton<IRepository<ScheduleDetailModel, ScheduleListModel>, ScheduleRepository>();

            services.AddSingleton<IMediator, Mediator>();

            services.AddSingleton<MainViewModel>();
            services.AddSingleton<IBandListViewModel, BandListViewModel>();
            services.AddFactory<IBandDetailViewModel, BandDetailViewModel>();
            services.AddSingleton<IStageListViewModel, StageListViewModel>();
            services.AddFactory<IStageDetailViewModel, StageDetailViewModel>();
            services.AddSingleton<IPhotoListViewModel, PhotoListViewModel>();
            services.AddFactory<IPhotoDetailViewModel, PhotoDetailViewModel>();
            services.AddSingleton<IScheduleListViewModel, ScheduleListViewModel>();
            services.AddFactory<IScheduleDetailViewModel, ScheduleDetailViewModel>();

            services.AddSingleton<Microsoft.EntityFrameworkCore.IDbContextFactory<FestivalDbContext>>(provider => new SqlServerDbContextFactory(configuration.GetConnectionString("DefaultConnection")));
        }
        protected override void OnStartup(StartupEventArgs e)
        {
            _host.Start();

            var dbContextFactory = _host.Services.GetRequiredService<Microsoft.EntityFrameworkCore.IDbContextFactory<FestivalDbContext>>();

#if DEBUG
            using (var dbx = dbContextFactory.CreateDbContext())
            {
                dbx.Database.Migrate();
            }
#endif

            var mainWindow = _host.Services.GetRequiredService<MainWindow>();
            mainWindow.Show();

            base.OnStartup(e);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            using (_host)
            {
                _host.StopAsync(TimeSpan.FromSeconds(5));
            }

            base.OnExit(e);
        }
    }
}
