using System;


namespace ICSproj.BL.Models
{
    public class ScheduleListModel : ModelBase
    {
        public Guid BandId { get; set; }
        public string BandName { get; set; }
        public Guid StageId { get; set; }
        public string StageName { get; set; }
        public DateTime PerformanceDateTime { get; set; }
        public TimeSpan PerformanceDuration { get; set; }

    }
}
