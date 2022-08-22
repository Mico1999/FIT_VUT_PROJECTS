using System;
using System.Collections.Generic;
using System.Text;

namespace ICSproj.DAL.Entities
{
    public class StageEntity : BaseEntity, IFestivalEntity, IEquatable<StageEntity>
    {

        public StageEntity(Guid id)
        {
            Id = id;
        }

        public StageEntity()
        { }

        public string Name { get; set; }
        public string Description { get; set; }
        public ICollection<PhotoEntity> Photos { get; set; } = new List<PhotoEntity>();
        public ICollection<ScheduleEntity> PerformanceMapping { get; set; } = new List<ScheduleEntity>();

        /**
         * Photos & PerformanceMapping are not working in this test
         */
        public bool Equals(StageEntity other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Name == other.Name && Description == other.Description;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((StageEntity) obj);
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(Name, Description, Photos, PerformanceMapping);
        }
    }
}
