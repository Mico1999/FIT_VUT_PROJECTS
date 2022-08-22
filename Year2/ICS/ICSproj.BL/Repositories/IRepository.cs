using System;
using System.Collections.Generic;


namespace ICSproj.BL.Repositories
{
    public interface IRepository<TDetailModel, TListModel>
    {
        ICollection<TListModel> GetAll();
        TDetailModel GetById(Guid id);
        TDetailModel InsertOrUpdate(TDetailModel model);
        bool Delete(Guid id);
    }
}
