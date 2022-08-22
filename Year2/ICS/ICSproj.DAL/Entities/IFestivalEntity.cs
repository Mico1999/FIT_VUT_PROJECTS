using System;
using System.Collections.Generic;
using System.Text;

namespace ICSproj.DAL.Entities
{
    public interface IFestivalEntity
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public ICollection<PhotoEntity> Photos { get; }
        public ICollection<ScheduleEntity> PerformanceMapping { get; }
    }
}
