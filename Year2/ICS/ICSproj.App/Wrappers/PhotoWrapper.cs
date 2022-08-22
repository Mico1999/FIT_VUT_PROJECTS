using System;
using ICSproj.BL.Models;

namespace ICSproj.App.Wrappers
{
    
    public class PhotoWrapper : ModelWrapper<PhotoDetailModel>
    {
        public PhotoWrapper(PhotoDetailModel model)
            : base(model)
        { }

        public byte[] Photo
        {
            get => GetValue<byte[]>();
            set => SetValue(value);
        }

        public Guid ForeignGuid
        {
            get => GetValue<Guid>(); 
            set => SetValue(value);
        }

        public static implicit operator PhotoWrapper(PhotoDetailModel detailModel)
            => new(detailModel);

        public static implicit operator PhotoDetailModel(PhotoWrapper wrapper)
            => wrapper.Model;
    }
}
