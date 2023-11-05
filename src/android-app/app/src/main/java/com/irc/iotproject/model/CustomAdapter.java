package com.irc.iotproject.model;


import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.irc.iotproject.R;

import java.util.List;

public class CustomAdapter extends RecyclerView.Adapter<CustomAdapter.ViewHolder> {

    private static final String TAG = "RecyclerAdapter";
    List<UnitItem> unitItems;

    public CustomAdapter(List<UnitItem> units) {
        this.unitItems = units;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        View view = layoutInflater.inflate(R.layout.row_item, parent, false);
        ViewHolder viewHolder = new ViewHolder(view);
        return viewHolder;
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        UnitItem unit = unitItems.get(position);
        holder.value.setText(unit.toString());
        holder.title.setText(unit.getTitle());
        holder.imageView.setImageResource(unit.getImage());
    }

    @Override
    public int getItemCount() {
        return unitItems.size();
    }

    class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

        ImageView imageView;
        TextView title, value;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);

            imageView = itemView.findViewById(R.id.imageView);
            title = itemView.findViewById(R.id.title);
            value = itemView.findViewById(R.id.value);

            itemView.setOnClickListener(this);

        }

        @Override
        public void onClick(View view) {
            Toast.makeText(view.getContext(), unitItems.get(getAdapterPosition()).getTitle(), Toast.LENGTH_SHORT).show();
        }
    }
}