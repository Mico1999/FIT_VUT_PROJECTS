using ICSproj.BL.Models;

namespace ICSproj.App.Messages
{
    public class DeleteMessage<T> : Message<T>
        where T : IModel
    {
    }
}
