using System.Collections.Generic;


namespace ICSproj.BL.Models
{
    public class BandListModel : ModelBase
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public List<PhotoListModel> Photos { get; set; }
        public List<ScheduleListModel> Schedule { get; set; }
    }
}
