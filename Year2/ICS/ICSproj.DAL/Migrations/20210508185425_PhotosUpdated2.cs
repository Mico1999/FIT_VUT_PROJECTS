using System;
using Microsoft.EntityFrameworkCore.Migrations;

namespace ICSproj.DAL.Migrations
{
    public partial class PhotosUpdated2 : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Extension",
                table: "Photos");

            migrationBuilder.AddColumn<Guid>(
                name: "ForeignGuid",
                table: "Photos",
                type: "uniqueidentifier",
                nullable: false,
                defaultValue: new Guid("00000000-0000-0000-0000-000000000000"));
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "ForeignGuid",
                table: "Photos");

            migrationBuilder.AddColumn<string>(
                name: "Extension",
                table: "Photos",
                type: "nvarchar(max)",
                nullable: true);
        }
    }
}
