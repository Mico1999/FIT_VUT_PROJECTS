using System.Collections.Generic;

namespace ICSproj.BL.Models
{
    public class BandDetailModel : ModelBase
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public List<PhotoDetailModel> Photos { get; set; }
        public List<ScheduleDetailModel> Schedule { get; set; }
        public string Genre { get; set; }
        public string OriginCountry { get; set; }
        public string DescriptionLong { get; set; }
    }
}
