using System;
using System.Collections.Generic;
using System.Text;

namespace ICSproj.DAL.Entities
{
    public abstract class BaseEntity : IEntity
    {
        public Guid Id { get; init; }
    }
}
