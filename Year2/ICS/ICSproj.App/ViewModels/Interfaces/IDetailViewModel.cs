using System;

namespace ICSproj.App.ViewModels.Interfaces
{
    public interface IDetailViewModel<TDetail> : IViewModel
    {
        TDetail Model { get; set; }

        void Load(Guid id);
    }
}
