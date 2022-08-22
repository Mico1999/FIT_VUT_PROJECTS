using System;
using System.Collections.Generic;
using System.Text;

namespace ICSproj.DAL.Entities
{
    public interface IEntity
    {
        Guid Id { get; }
    }
}
