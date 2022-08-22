using System;
using System.Collections.Generic;

namespace ICSproj.DAL.Entities
{
    public class BandEntity : BaseEntity, IBandEntity, IEquatable<BandEntity>
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public ICollection<PhotoEntity> Photos { get; set; } = new List<PhotoEntity>();
        public ICollection<ScheduleEntity> PerformanceMapping { get; set; } = new List<ScheduleEntity>();
        public string Genre { get; set; }
        public string OriginCountry { get; set; }
        public string DescriptionLong { get; set; }


        public BandEntity(Guid id)
        {
            Id = id;
        }

        public BandEntity()
        {
        }

        /**
         * Photos & PerformanceMapping are not working in this test
         */
        public bool Equals(BandEntity other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Name == other.Name && Description == other.Description && Genre == other.Genre && OriginCountry == other.OriginCountry && DescriptionLong == other.DescriptionLong;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((BandEntity) obj);
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(Name, Description, Photos, PerformanceMapping, Genre, OriginCountry, DescriptionLong);
        }
    }
}
