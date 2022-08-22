using System.Collections.Generic;


namespace ICSproj.BL.Models
{
    public class StageDetailModel : ModelBase
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public List<PhotoDetailModel> Photos { get; set; }
        public List<ScheduleDetailModel> Schedule { get; set; } 
    }
}
