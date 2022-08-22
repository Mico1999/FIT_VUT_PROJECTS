using System;


namespace ICSproj.BL.Models
{
    public abstract class ModelBase : IModel
    {
        public Guid Id { get; set; }
    }
}
