using System;
using System.Collections.Generic;
using System.Text;

namespace ICSproj.DAL.Entities
{
    public interface IBandEntity : IFestivalEntity
    {
        public string Genre { get; set; }
        public string OriginCountry { get; set; }
        public string DescriptionLong { get; set; }
    }
}
