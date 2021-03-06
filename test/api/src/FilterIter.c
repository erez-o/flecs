#include <api.h>

void FilterIter_iter_one_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w_count(world, Position, 3);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_filter_iter_t it = ecs_filter_iter(world, &(ecs_filter_t){
        .include = ecs_type(Position),
    });

    int table_count = 0;
    int entity_count = 0;

    while (ecs_filter_next(&it)) {
        table_count ++;
        entity_count += it.rows.count;
        
        test_assert(ecs_table_type(&it.rows) == ecs_type(Position));
        Position *row = ecs_table_column(&it.rows, 0);
        test_assert(row != NULL);

        for (i = 0; i < it.rows.count; i ++) {
            test_int(row[i].x, i);
            test_int(row[i].y, i * 2);
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 3);
    
    ecs_fini(world);
}

void FilterIter_iter_two_tables() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Movable, Position, Velocity);

    ecs_entity_t e = ecs_new_w_count(world, Position, 3);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    e = ecs_new_w_count(world, Movable, 3);
    test_assert(e != 0);

    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
    }    

    ecs_filter_iter_t it = ecs_filter_iter(world, &(ecs_filter_t){
        .include = ecs_type(Position),
    });

    int table_count = 0;
    int entity_count = 0;

    while (ecs_filter_next(&it)) {
        table_count ++;
        entity_count += it.rows.count;
        
        Position *row = ecs_table_column(&it.rows, 0);
        test_assert(row != NULL);

        for (i = 0; i < it.rows.count; i ++) {
            test_int(row[i].x, i);
            test_int(row[i].y, i * 2);
        }
    }

    test_int(table_count, 2);
    test_int(entity_count, 6);
    
    ecs_fini(world);
}

void FilterIter_iter_two_comps() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Movable, Position, Velocity);

    ecs_entity_t e = ecs_new_w_count(world, Movable, 3);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
        ecs_set(world, e + i, Velocity, {i + 1, i * 2 + 1});
    }

    ecs_filter_iter_t it = ecs_filter_iter(world, &(ecs_filter_t){
        .include = ecs_type(Movable),
    });

    int table_count = 0;
    int entity_count = 0;

    while (ecs_filter_next(&it)) {
        table_count ++;
        entity_count += it.rows.count;

        ecs_type_t table_type = ecs_table_type(&it.rows);
        test_assert(ecs_type_get_entity(world, table_type, 0) == ecs_entity(Position));
        test_assert(ecs_type_get_entity(world, table_type, 1) == ecs_entity(Velocity));
        
        Position *p_row = ecs_table_column(&it.rows, 0);
        test_assert(p_row != NULL);

        Velocity *v_row = ecs_table_column(&it.rows, 1);
        test_assert(v_row != NULL);        

        for (i = 0; i < it.rows.count; i ++) {
            test_int(p_row[i].x, i);
            test_int(p_row[i].y, i * 2);

            test_int(v_row[i].x, 1 + i);
            test_int(v_row[i].y, 1 + i * 2);
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 3);
    
    ecs_fini(world);
}

void FilterIter_iter_snapshot_one_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w_count(world, Position, 3);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);
    test_assert(s != NULL);

    ecs_type_filter_t filter = { ecs_type(Position) };

    /* First clear data from the world to ensure that we're not accidentally
     * iterating from the world */
    ecs_delete_w_filter(world, &filter);

    /* Ensure world is empty */
    test_int( ecs_count(world, Position), 0);

    ecs_filter_iter_t it = ecs_snapshot_filter_iter(world, s, &filter);
    int table_count = 0;
    int entity_count = 0;

    while (ecs_filter_next(&it)) {
        table_count ++;
        entity_count += it.rows.count;

        ecs_type_t table_type = ecs_table_type(&it.rows);
        test_assert(ecs_type_get_entity(world, table_type, 0) == ecs_entity(Position));
        
        Position *p_row = ecs_table_column(&it.rows, 0);
        test_assert(p_row != NULL);    

        for (i = 0; i < it.rows.count; i ++) {
            test_int(p_row[i].x, i);
            test_int(p_row[i].y, i * 2);
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 3);

    ecs_snapshot_free(world, s);
    
    ecs_fini(world);
}

void FilterIter_iter_snapshot_two_tables() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Movable, Position, Velocity);

    ecs_entity_t e = ecs_new_w_count(world, Position, 3);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    e = ecs_new_w_count(world, Movable, 3);
    test_assert(e != 0);

    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);
    test_assert(s != NULL);

    ecs_type_filter_t filter = { ecs_type(Position) };

    /* First clear data from the world to ensure that we're not accidentally
     * iterating from the world */
    ecs_delete_w_filter(world, &filter);

    /* Ensure world is empty */
    test_int( ecs_count(world, Position), 0);

    ecs_filter_iter_t it = ecs_snapshot_filter_iter(world, s, &filter);
    int table_count = 0;
    int entity_count = 0;

    while (ecs_filter_next(&it)) {
        table_count ++;
        entity_count += it.rows.count;

        ecs_type_t table_type = ecs_table_type(&it.rows);
        test_assert(ecs_type_get_entity(world, table_type, 0) == ecs_entity(Position));
        
        Position *p_row = ecs_table_column(&it.rows, 0);
        test_assert(p_row != NULL);     

        for (i = 0; i < it.rows.count; i ++) {
            test_int(p_row[i].x, i);
            test_int(p_row[i].y, i * 2);
        }
    }

    test_int(table_count, 2);
    test_int(entity_count, 6);

    ecs_snapshot_free(world, s);
    
    ecs_fini(world);
}

void FilterIter_iter_snapshot_two_comps() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Movable, Position, Velocity);

    ecs_entity_t e = ecs_new_w_count(world, Movable, 3);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
        ecs_set(world, e + i, Velocity, {i + 1, i * 2 + 1});
    }

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);
    test_assert(s != NULL);

    ecs_type_filter_t filter = { ecs_type(Movable) };

    /* First clear data from the world to ensure that we're not accidentally
     * iterating from the world */
    ecs_delete_w_filter(world, &filter);

    /* Ensure world is empty */
    test_int( ecs_count(world, Movable), 0);

    ecs_filter_iter_t it = ecs_snapshot_filter_iter(world, s, &filter);
    int table_count = 0;
    int entity_count = 0;

    while (ecs_filter_next(&it)) {
        table_count ++;
        entity_count += it.rows.count;

        ecs_type_t table_type = ecs_table_type(&it.rows);
        test_assert(ecs_type_get_entity(world, table_type, 0) == ecs_entity(Position));
        test_assert(ecs_type_get_entity(world, table_type, 1) == ecs_entity(Velocity));
        
        Position *p_row = ecs_table_column(&it.rows, 0);
        test_assert(p_row != NULL);

        Velocity *v_row = ecs_table_column(&it.rows, 1);
        test_assert(v_row != NULL);        

        for (i = 0; i < it.rows.count; i ++) {
            test_int(p_row[i].x, i);
            test_int(p_row[i].y, i * 2);

            test_int(v_row[i].x, 1 + i);
            test_int(v_row[i].y, 1 + i * 2);
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 3);

    ecs_snapshot_free(world, s);
    
    ecs_fini(world);
}

void FilterIter_iter_snapshot_filtered_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Movable, Position, Velocity);

    ecs_entity_t e = ecs_new_w_count(world, Movable, 3);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    e = ecs_new_w_count(world, Position, 3);
    test_assert(e != 0);

    for (i = 0; i < 3; i ++) {
        ecs_set(world, e + i, Position, {i, i * 2});
    }    

    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t){
        .exclude = ecs_type(Velocity)
    });
    test_assert(s != NULL);

    ecs_type_filter_t filter = { ecs_type(Position) };

    /* First clear data from the world to ensure that we're not accidentally
     * iterating from the world */
    ecs_delete_w_filter(world, &filter);

    /* Ensure world is empty */
    test_int( ecs_count(world, Position), 0);

    ecs_filter_iter_t it = ecs_snapshot_filter_iter(world, s, &filter);
    int table_count = 0;
    int entity_count = 0;

    while (ecs_filter_next(&it)) {
        table_count ++;
        entity_count += it.rows.count;

        ecs_type_t table_type = ecs_table_type(&it.rows);
        test_assert(ecs_type_get_entity(world, table_type, 0) == ecs_entity(Position));
        
        Position *p_row = ecs_table_column(&it.rows, 0);
        test_assert(p_row != NULL);

        for (i = 0; i < it.rows.count; i ++) {
            test_int(p_row[i].x, i);
            test_int(p_row[i].y, i * 2);
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 3);

    ecs_snapshot_free(world, s);
    
    ecs_fini(world);
}
